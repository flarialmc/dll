#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "DepthOfFieldHelper.hpp"
#include "../MotionBlur/MotionBlur.hpp"

static const XMFLOAT4 quadVertices[] =
{
    XMFLOAT4(1.0, -1.0, 0.0, 1.0),
    XMFLOAT4(-1.0, -1.0, 0.0, 1.0),
    XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
    XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
    XMFLOAT4(1.0, 1.0, 0.0, 1.0),
    XMFLOAT4(1.0, -1.0, 0.0, 1.0)
};

const char* dofVertexShaderSrc = R"(
struct VS_INPUT {
    float4 pos : POSITION;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.pos = input.pos;
    output.uv = float2((input.pos.x + 1.0) * 0.5, (1.0 - input.pos.y) * 0.5);
    return output;
}
)";

const char* dofPixelShaderSrc = R"(
// Compile target: ps_5_0

cbuffer DepthOfFieldBuffer : register(b0)
{
    float2 resolution;
    float2 halfPixel;
    float intensity;
    float focusRange;
    float focusDistance;
    float autoFocus;
    float msaaLevel;
    float3 _padding;
};

Texture2D    colorTexture   : register(t0);
Texture2D depthTexture : register(t1);
Texture2DMS<float> msaaDepth : register(t2); // supports up to 4 samples
SamplerState linearSampler  : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// small disk sample offsets (12 samples)
static const float2 SAMPLE_OFFSETS[12] =
{
    float2( 0.000,  0.000),
    float2( 0.530,  0.000),
    float2(-0.530,  0.000),
    float2( 0.000,  0.530),
    float2( 0.000, -0.530),
    float2( 0.375,  0.375),
    float2(-0.375,  0.375),
    float2( 0.375, -0.375),
    float2(-0.375, -0.375),
    float2( 0.800,  0.200),
    float2(-0.200,  0.800),
    float2(-0.800, -0.200)
};

float GetDepthAtPixel(int2 pixCoord, float2 uv)
{
    // If MSAA is enabled (msaaLevel > 1) sample msaa depth and average samples.
    if (msaaLevel > 1.0f)
    {
        // msaaLevel may be 2,4,... clamp to available template samples (1..4).
        int sampleCount = (int)msaaLevel;
        sampleCount = clamp(sampleCount, 1, 4);

        float dsum = 0.0f;
        [unroll]
        for (int i = 0; i < 4; ++i) // iterate up to template count
        {
            if (i >= sampleCount) break;
            dsum += msaaDepth.Load(pixCoord, i);
        }
        return dsum / (float)sampleCount;
    }
    else
    {
        // single-sample depth: sample with SampleLevel using UV
        return depthTexture.SampleLevel(linearSampler, uv, 0.0f).r;
    }
}

// simple Gaussian-like weight by distance (not normalized)
float WeightByDist(float2 offset)
{
    float r = length(offset);
    // sharper falloff for larger radii
    return exp(-r * r * 4.0);
}

float4 main(PSInput IN) : SV_TARGET
{
    // pixel integer coordinate for Load() on MSAA resource
    int2 pixelCoord = int2(IN.pos.xy);

    // Get center depth (auto-focus override if requested)
    float centerDepth = GetDepthAtPixel(pixelCoord, IN.uv);
    float focal = focusDistance;
    if (autoFocus > 0.5f)
    {
        // Use screen center for autofocus instead of current pixel
        int2 centerCoord = int2(resolution * 0.5);
        float2 centerUV = float2(0.5, 0.5);
        focal = GetDepthAtPixel(centerCoord, centerUV);
    }

    // safety clamp for focusRange
    float range = max(focusRange, 1e-5);

    // circle of confusion metric [0..1], scaled by intensity
    float depthDiff = abs(centerDepth - focal);
    float coc = saturate(depthDiff / range) * intensity;

    // For debugging - ensure some blur is always applied when intensity > 0
    if (intensity > 0.01) {
        coc = max(coc, 0.1); // minimum blur amount for testing
    }

    // if very small or intensity is 0, return sharp sample
    if (coc <= 0.001 || intensity <= 0.001)
    {
        return colorTexture.SampleLevel(linearSampler, IN.uv, 0.0f);
    }

    // convert coc to pixel blur radius (tweak multiplier as desired)
    // larger multiplier => stronger blur for same coc
    float radiusPx = coc * 15.0; // increased for more visible effect

    // accumulate samples
    float3 accum = 0.0;
    float totalWeight = 0.0;

    // center sample (always)
    float4 c = colorTexture.SampleLevel(linearSampler, IN.uv, 0.0f);
    float w0 = 1.0;
    accum += c.rgb * w0;
    totalWeight += w0;

    // sample disk offsets
    [unroll]
    for (int i = 1; i < 12; ++i)
    {
        float2 off = SAMPLE_OFFSETS[i] * radiusPx;
        // convert offset in pixels -> normalized uv
        float2 uvOff = IN.uv + off / resolution;

        // Clamp UV coordinates to prevent sampling outside texture bounds
        uvOff = clamp(uvOff, 0.0, 1.0);

        float w = WeightByDist(SAMPLE_OFFSETS[i]); // weight based on normalized offset
        float4 col = colorTexture.SampleLevel(linearSampler, uvOff, 0.0f);
        accum += col.rgb * w;
        totalWeight += w;
    }

    float3 finalColor = accum / max(totalWeight, 1e-6);

    return float4(finalColor, 1.0);
}

)";

ID3DBlob* DofTryCompileShader(const char* pSrcData, const char* pTarget)
{
    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;
    HRESULT hr = D3DCompile(pSrcData, strlen(pSrcData), nullptr, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) {
            errorBlob->Release();
        }
        throw std::logic_error("Failed to compile shader!");
    }
    return shaderBlob;
}

void DepthOfFieldHelper::InitializePipeline()
{
    if (!SwapchainHook::d3d11Device) {
        throw std::runtime_error("D3D11 device is null");
    }

    CD3D11_BUFFER_DESC cbd(sizeof(DepthOfFieldInputBuffer), D3D11_BIND_CONSTANT_BUFFER);
    CD3D11_BUFFER_DESC cbdVertex(sizeof(quadVertices), D3D11_BIND_VERTEX_BUFFER);

    SwapchainHook::d3d11Device->CreateBuffer(&cbd, nullptr, &pConstantBuffer);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {quadVertices, 0, 0};
    SwapchainHook::d3d11Device->CreateBuffer(&cbdVertex, &vertexBufferData, &pVertexBuffer);

    ID3DBlob* shaderBlob = DofTryCompileShader(dofPixelShaderSrc, "ps_5_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pDepthOfFieldShader);

    shaderBlob = DofTryCompileShader(dofVertexShaderSrc, "vs_5_0");
    SwapchainHook::d3d11Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pVertexShader);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    SwapchainHook::d3d11Device->CreateInputLayout(ied, 1, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pInputLayout);

    D3D11_SAMPLER_DESC sd{};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SwapchainHook::d3d11Device->CreateSamplerState(&sd, &pSampler);

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);

}

void DepthOfFieldHelper::SetupShaderResources(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* pColorSRV)
{
    pContext->PSSetShaderResources(0, 1, &pColorSRV);

    if (isMSAADepth) {
        ID3D11ShaderResourceView* nullSRV = nullptr;
        pContext->PSSetShaderResources(1, 1, &nullSRV);
        pContext->PSSetShaderResources(2, 1, &pDepthMapSRV);
    } else {
        pContext->PSSetShaderResources(1, 1, &pDepthMapSRV);
        ID3D11ShaderResourceView* nullSRV = nullptr;
        pContext->PSSetShaderResources(2, 1, &nullSRV);
    }
}

void DepthOfFieldHelper::RenderToRTV(ID3D11RenderTargetView* pRenderTargetView, ID3D11ShaderResourceView* pShaderResourceView, XMFLOAT2 rtvSize)
{
    winrt::com_ptr<ID3D11DeviceContext> pContext = SwapchainHook::context;
    if (!pContext) return;

    //pContext->OMSetDepthStencilState(pDepthStencilState, 0);
    //pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    pContext->RSSetState(pRasterizerState);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    pContext->PSSetShaderResources(0, 1, &nullSRV);
    pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

    constantBuffer.resolution = rtvSize;
    constantBuffer.halfpixel = XMFLOAT2(0.5f / rtvSize.x, 0.5f / rtvSize.y);
    pContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);

    pContext->IASetInputLayout(pInputLayout);
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(pVertexShader, nullptr, 0);
    pContext->PSSetShader(pDepthOfFieldShader, nullptr, 0);
    pContext->PSSetSamplers(0, 1, &pSampler);
    pContext->PSSetSamplers(1, 1, &pSampler);
    pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

    SetupShaderResources(pContext.get(), pShaderResourceView);

    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = rtvSize.x;
    viewport.Height = rtvSize.y;
    viewport.MaxDepth = 1.0f;
    pContext->RSSetViewports(1, &viewport);

    FLOAT backgroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    pContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);
    pContext->Draw(sizeof(quadVertices) / sizeof(quadVertices[0]), 0);

    ID3D11RenderTargetView* nullRTV = nullptr;
    pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
}

void DepthOfFieldHelper::Cleanup()
{
    if (pDepthStencilState) { pDepthStencilState->Release(); pDepthStencilState = nullptr; }
    if (pBlendState) { pBlendState->Release(); pBlendState = nullptr; }
    if (pRasterizerState) { pRasterizerState->Release(); pRasterizerState = nullptr; }
    if (pDepthOfFieldShader) { pDepthOfFieldShader->Release(); pDepthOfFieldShader = nullptr; }
    if (pVertexShader) { pVertexShader->Release(); pVertexShader = nullptr; }
    if (pInputLayout) { pInputLayout->Release(); pInputLayout = nullptr; }
    if (pSampler) { pSampler->Release(); pSampler = nullptr; }
    if (pVertexBuffer) { pVertexBuffer->Release(); pVertexBuffer = nullptr; }
    if (pConstantBuffer) { pConstantBuffer->Release(); pConstantBuffer = nullptr; }
    if (pDepthMapSRV) { pDepthMapSRV->Release(); pDepthMapSRV = nullptr; }
}

void DepthOfFieldHelper::RenderDepthOfField(ID3D11RenderTargetView* pDstRenderTargetView, float intensity, float focusRange, float focusDistance, bool autoFocus)
{
    if (intensity <= 0 || !SwapchainHook::GetBackbuffer() || !pDepthMapSRV) return;

    winrt::com_ptr<ID3D11ShaderResourceView> pOrigShaderResourceView = MotionBlur::BackbufferToSRVExtraMode();
    if (!pOrigShaderResourceView) return;

    //if (!EnsureIntermediateTextures(MC::windowSize.x, MC::windowSize.y)) return;

    constantBuffer.intensity = intensity;
    constantBuffer.focusRange = focusRange;
    constantBuffer.focusDistance = focusDistance;
    constantBuffer.autoFocus = autoFocus ? 1.0f : 0.0f;

    if (isMSAADepth) {
        constantBuffer.msaaLevel = static_cast<float>(msaaSampleCount);
    } else {
        constantBuffer.msaaLevel = 0.5f;
    }

    XMFLOAT2 renderSize = XMFLOAT2(static_cast<float>(MC::windowSize.x), static_cast<float>(MC::windowSize.y));
    RenderToRTV(pDstRenderTargetView, pOrigShaderResourceView.get(), renderSize);
}