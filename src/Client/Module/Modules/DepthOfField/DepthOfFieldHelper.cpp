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

float4 main(VS_INPUT input) : SV_POSITION {
    return input.pos;
}
)";

const char* dofHorizontalShaderSrc = R"(
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

SamplerState colorSampler : register(s0);
SamplerState depthSampler : register(s1);

Texture2D colorTexture : register(t0);
Texture2D depthTexture : register(t1);
Texture2DMS<float> depthTextureMSAA : register(t2);

float SampleDepth(float2 uv, int2 pixelCoord)
{
    if (msaaLevel < 0.5) {
        return depthTexture.Sample(depthSampler, uv).r;
    } else {
        uint width, height, sampleCount;
        depthTextureMSAA.GetDimensions(width, height, sampleCount);

        float depthSum = 0.0;
        for (uint i = 0; i < sampleCount; i++) {
            depthSum += depthTextureMSAA.Load(pixelCoord, i).r;
        }
        return depthSum / float(sampleCount);
    }
}

float LinearizeDepth(float depth)
{
    float nearPlane = 0.1;
    float farPlane = 2000.0;
    float linearDepth = (2.0 * nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    return linearDepth * 100.0;
}

float CalculateCoC(float2 uv, float depth)
{
    float linearDepth = LinearizeDepth(depth);
    float targetFocusDistance;

    if (autoFocus > 0.5) {
        float2 screenCenter = float2(0.5, 0.5);
        int2 centerPixel = int2(screenCenter * resolution);
        float centerDepth = SampleDepth(screenCenter, centerPixel);
        targetFocusDistance = LinearizeDepth(centerDepth);
    } else {
        targetFocusDistance = focusDistance;
    }

    float distance = abs(linearDepth - targetFocusDistance);
    float normalizedDistance = distance / focusRange;

    return smoothstep(0.0, 1.0, normalizedDistance) * intensity;
}

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    int2 pixelCoord = int2(screenSpace.xy);

    float depth = SampleDepth(uv, pixelCoord);
    float coc = CalculateCoC(uv, depth);

    if (coc < 0.01) {
        return colorTexture.Sample(colorSampler, uv);
    }

    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    float weightSum = 0.0;

    int sampleCount = clamp((int)(coc * 8.0 + 3.0), 3, 15);
    int halfSamples = sampleCount / 2;

    for (int x = -halfSamples; x <= halfSamples; x++) {
        float weight = 1.0 - abs(x) / (float)(halfSamples + 1);
        float2 sampleOffset = float2(x * halfPixel.x * coc, 0.0);
        colorSum += colorTexture.Sample(colorSampler, uv + sampleOffset) * weight;
        weightSum += weight;
    }

    return colorSum / weightSum;
}
)";

const char* dofVerticalShaderSrc = R"(
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

SamplerState colorSampler : register(s0);
SamplerState depthSampler : register(s1);

Texture2D colorTexture : register(t0);
Texture2D depthTexture : register(t1);
Texture2DMS<float> depthTextureMSAA : register(t2);

float SampleDepth(float2 uv, int2 pixelCoord)
{
    if (msaaLevel < 0.5) {
        return depthTexture.Sample(depthSampler, uv).r;
    } else {
        uint width, height, sampleCount;
        depthTextureMSAA.GetDimensions(width, height, sampleCount);

        float depthSum = 0.0;
        for (uint i = 0; i < sampleCount; i++) {
            depthSum += depthTextureMSAA.Load(pixelCoord, i).r;
        }
        return depthSum / float(sampleCount);
    }
}

float LinearizeDepth(float depth)
{
    float nearPlane = 0.1;
    float farPlane = 2000.0;
    float linearDepth = (2.0 * nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    return linearDepth * 100.0;
}

float CalculateCoC(float2 uv, float depth)
{
    float linearDepth = LinearizeDepth(depth);
    float targetFocusDistance;

    if (autoFocus > 0.5) {
        float2 screenCenter = float2(0.5, 0.5);
        int2 centerPixel = int2(screenCenter * resolution);
        float centerDepth = SampleDepth(screenCenter, centerPixel);
        targetFocusDistance = LinearizeDepth(centerDepth);
    } else {
        targetFocusDistance = focusDistance;
    }

    float distance = abs(linearDepth - targetFocusDistance);
    float normalizedDistance = distance / focusRange;

    return smoothstep(0.0, 1.0, normalizedDistance) * intensity;
}

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    int2 pixelCoord = int2(screenSpace.xy);

    float depth = SampleDepth(uv, pixelCoord);
    float coc = CalculateCoC(uv, depth);

    if (coc < 0.01) {
        return colorTexture.Sample(colorSampler, uv);
    }

    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    float weightSum = 0.0;

    int sampleCount = clamp((int)(coc * 8.0 + 3.0), 3, 15);
    int halfSamples = sampleCount / 2;

    for (int y = -halfSamples; y <= halfSamples; y++) {
        float weight = 1.0 - abs(y) / (float)(halfSamples + 1);
        float2 sampleOffset = float2(0.0, y * halfPixel.y * coc);
        colorSum += colorTexture.Sample(colorSampler, uv + sampleOffset) * weight;
        weightSum += weight;
    }

    return colorSum / weightSum;
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

    ID3DBlob* shaderBlob = DofTryCompileShader(dofHorizontalShaderSrc, "ps_5_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pHorizontalShader);

    shaderBlob = DofTryCompileShader(dofVerticalShaderSrc, "ps_5_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pVerticalShader);

    shaderBlob = DofTryCompileShader(dofVertexShaderSrc, "vs_5_0");
    SwapchainHook::d3d11Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pVertexShader);

    D3D11_INPUT_ELEMENT_DESC ied = {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
    SwapchainHook::d3d11Device->CreateInputLayout(&ied, 1, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pInputLayout);

    D3D11_SAMPLER_DESC sd{};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SwapchainHook::d3d11Device->CreateSamplerState(&sd, &pSampler);

    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    SwapchainHook::d3d11Device->CreateDepthStencilState(&dsd, &pDepthStencilState);

    D3D11_BLEND_DESC bd{};
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = false;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    SwapchainHook::d3d11Device->CreateBlendState(&bd, &pBlendState);

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);}

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

    pContext->OMSetDepthStencilState(pDepthStencilState, 0);
    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
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
    // Shader will be set in RenderDepthOfField based on pass
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
    if (pHorizontalShader) { pHorizontalShader->Release(); pHorizontalShader = nullptr; }
    if (pVerticalShader) { pVerticalShader->Release(); pVerticalShader = nullptr; }
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
    if (!SwapchainHook::ExtraSavedD3D11BackBuffer) return;

    winrt::com_ptr<ID3D11DeviceContext> pContext = SwapchainHook::context;
    if (!pContext) return;

    constantBuffer.intensity = intensity;
    constantBuffer.focusRange = focusRange;
    constantBuffer.focusDistance = focusDistance;
    constantBuffer.autoFocus = autoFocus ? 1.0f : 0.0f;

    if (isMSAADepth) {
        if (msaaSampleCount == 2) constantBuffer.msaaLevel = 1.0f;
        else if (msaaSampleCount == 4) constantBuffer.msaaLevel = 2.0f;
        else if (msaaSampleCount == 8) constantBuffer.msaaLevel = 3.0f;
        else constantBuffer.msaaLevel = 0.0f;
    } else {
        constantBuffer.msaaLevel = 0.0f;
    }

    XMFLOAT2 renderSize = XMFLOAT2(static_cast<float>(MC::windowSize.x), static_cast<float>(MC::windowSize.y));

    // Ensure intermediate textures exist
    D3D11_TEXTURE2D_DESC desc;
    SwapchainHook::GetBackbuffer()->GetDesc(&desc);

    if (currentTextureWidth != desc.Width || currentTextureHeight != desc.Height ||
        !pIntermediateTexture1 || !pIntermediateRTV1 || !pIntermediateSRV1) {

        // Release existing textures
        if (pIntermediateSRV1) { pIntermediateSRV1->Release(); pIntermediateSRV1 = nullptr; }
        if (pIntermediateRTV1) { pIntermediateRTV1->Release(); pIntermediateRTV1 = nullptr; }
        if (pIntermediateTexture1) { pIntermediateTexture1->Release(); pIntermediateTexture1 = nullptr; }

        // Create new intermediate texture
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = desc.Width;
        textureDesc.Height = desc.Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        SwapchainHook::d3d11Device->CreateTexture2D(&textureDesc, nullptr, &pIntermediateTexture1);
        SwapchainHook::d3d11Device->CreateRenderTargetView(pIntermediateTexture1, nullptr, &pIntermediateRTV1);
        SwapchainHook::d3d11Device->CreateShaderResourceView(pIntermediateTexture1, nullptr, &pIntermediateSRV1);

        currentTextureWidth = desc.Width;
        currentTextureHeight = desc.Height;
    }

    // Clear previous shader resource bindings
    ID3D11ShaderResourceView* nullSRV = nullptr;
    pContext->PSSetShaderResources(0, 1, &nullSRV);

    // Horizontal pass
    pContext->PSSetShader(pHorizontalShader, nullptr, 0);
    RenderToRTV(pIntermediateRTV1, pOrigShaderResourceView.get(), renderSize);

    // Clear binding again
    pContext->PSSetShaderResources(0, 1, &nullSRV);

    // Vertical pass (final)
    pContext->PSSetShader(pVerticalShader, nullptr, 0);
    RenderToRTV(pDstRenderTargetView, pIntermediateSRV1, renderSize);
}