#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "DepthOfFieldHelper.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../../Client/Client.hpp"


#define BLUR_OFFSET 10

static const XMFLOAT4 quadVertices[] =
    {
        XMFLOAT4(1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, -1.0, 0.0, 1.0)};

const char *dofVertexShaderSrc = "struct VS_INPUT {\
    float4 pos : POSITION;\
};\
\
float4 main(VS_INPUT input) : SV_POSITION {\
    return input.pos;\
}";

const char *dofGaussianBlurHorizontalShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
    float intensity;
    float focusRange;
    float focusDistance;
    float autoFocus;
    float _padding;
    float _padding2;
};

sampler sampler0 : register(s0);
sampler sampler1 : register(s1);
Texture2D texture0 : register(t0);  // Scene color
Texture2D texture1 : register(t1);  // Depth buffer

float LinearizeDepth(float depth)
{
    float nearPlane = 0.1;
    float farPlane = 2000.0;
    float linearDepth = (2.0 * nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    // Scale to a more usable range for depth of field calculations
    return linearDepth * 100.0;
}

float CalculateCoC(float2 uv, float depth)
{
    float linearDepth = LinearizeDepth(depth);
    float targetFocusDistance;

    if (autoFocus > 0.5) {
        // Auto-focus: sample depth at screen center
        float2 screenCenter = float2(0.5, 0.5);
        float centerDepth = texture1.Sample(sampler1, screenCenter).r;
        targetFocusDistance = LinearizeDepth(centerDepth);
    } else {
        // Manual focus: use focus distance setting
        targetFocusDistance = focusDistance;
    }

    float distance = abs(linearDepth - targetFocusDistance);
    float normalizedDistance = distance / focusRange;

    return smoothstep(0.0, 1.0, normalizedDistance) * intensity * 2.0;
}

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;

    // Sample depth and calculate circle of confusion
    float depth = texture1.Sample(sampler1, uv).r;
    float coc = CalculateCoC(uv, depth);

    // If CoC is very small, return original color (in focus)
    if (coc < 0.01)
    {
        return texture0.Sample(sampler0, uv);
    }

    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    float weightSum = 0.0;

    int sampleCount = clamp((int)(coc * 8.0 + 3.0), 3, 15);
    float sigma = coc * 0.5 + 0.1;
    float twoSigmaSq = 2.0 * sigma * sigma;

    int halfSamples = sampleCount / 2;

    for (int i = -halfSamples; i <= halfSamples; i++)
    {
        float x = (float)i;
        float weight = exp(-(x * x) / twoSigmaSq);
        float2 sampleOffset = float2(x * halfPixel.x * coc * intensity, 0.0);

        colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
        weightSum += weight;
    }

    return colorSum / weightSum;
}
)";

const char *dofGaussianBlurVerticalShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
    float intensity;
    float focusRange;
    float focusDistance;
    float autoFocus;
    float _padding;
    float _padding2;
};

sampler sampler0 : register(s0);
sampler sampler1 : register(s1);
Texture2D texture0 : register(t0);  // Horizontally blurred color
Texture2D texture1 : register(t1);  // Depth buffer

float LinearizeDepth(float depth)
{
    float nearPlane = 0.1;
    float farPlane = 2000.0;
    float linearDepth = (2.0 * nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    // Scale to a more usable range for depth of field calculations
    return linearDepth * 100.0;
}

float CalculateCoC(float2 uv, float depth)
{
    float linearDepth = LinearizeDepth(depth);
    float targetFocusDistance;

    if (autoFocus > 0.5) {
        // Auto-focus: sample depth at screen center
        float2 screenCenter = float2(0.5, 0.5);
        float centerDepth = texture1.Sample(sampler1, screenCenter).r;
        targetFocusDistance = LinearizeDepth(centerDepth);
    } else {
        // Manual focus: use focus distance setting
        targetFocusDistance = focusDistance;
    }

    float distance = abs(linearDepth - targetFocusDistance);
    float normalizedDistance = distance / focusRange;

    return smoothstep(0.0, 1.0, normalizedDistance) * intensity * 2.0;
}

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;

    // Sample depth and calculate circle of confusion
    float depth = texture1.Sample(sampler1, uv).r;
    float coc = CalculateCoC(uv, depth);

    // If CoC is very small, return original color (in focus)
    if (coc < 0.01)
    {
        return texture0.Sample(sampler0, uv);
    }

    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    float weightSum = 0.0;

    int sampleCount = clamp((int)(coc * 8.0 + 3.0), 3, 15);
    float sigma = coc * 0.5 + 0.1;
    float twoSigmaSq = 2.0 * sigma * sigma;

    int halfSamples = sampleCount / 2;

    for (int i = -halfSamples; i <= halfSamples; i++)
    {
        float y = (float)i;
        float weight = exp(-(y * y) / twoSigmaSq);
        float2 sampleOffset = float2(0.0, y * halfPixel.y * coc * intensity);

        colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
        weightSum += weight;
    }

    return colorSum / weightSum;
}
)";

ID3DBlob *DofTryCompileShader(const char *pSrcData, const char *pTarget)
{
    HRESULT hr;

    ID3DBlob *shaderBlob;
    ID3DBlob *errorBlob;
    hr = D3DCompile(pSrcData, strlen(pSrcData), nullptr, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob) {
            errorBlob->Release();
        }
        throw std::logic_error("Failed to compile shader!");
    }
    return shaderBlob;
}

void DepthOfFieldHelper::InitializePipeline()
{
    HRESULT hr;
    ID3D11DeviceContext* pContext = SwapchainHook::context;


    // byteWidth has to be a multiple of 32, BlurInputBuffer has a size of 24
    CD3D11_BUFFER_DESC cbd(
        sizeof(DepthOfFieldInputBuffer),
        D3D11_BIND_CONSTANT_BUFFER);
    CD3D11_BUFFER_DESC cbdVertex(
        sizeof(quadVertices),
        D3D11_BIND_VERTEX_BUFFER);

    SwapchainHook::d3d11Device->CreateBuffer(
        &cbd,
        nullptr,
        &pConstantBuffer);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {quadVertices, 0, 0};

    SwapchainHook::d3d11Device->CreateBuffer(
        &cbdVertex,
        &vertexBufferData,
        &pVertexBuffer);

    ID3DBlob *shaderBlob = DofTryCompileShader(dofGaussianBlurHorizontalShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pGaussianBlurHorizontalShader);

    shaderBlob = DofTryCompileShader(dofGaussianBlurVerticalShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pGaussianBlurVerticalShader);

    shaderBlob = DofTryCompileShader(dofVertexShaderSrc, "vs_4_0");
    SwapchainHook::d3d11Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pVertexShader);

    D3D11_INPUT_ELEMENT_DESC ied =
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
    SwapchainHook::d3d11Device->CreateInputLayout(&ied, 1, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pInputLayout);

    D3D11_SAMPLER_DESC sd{};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SwapchainHook::d3d11Device->CreateSamplerState(&sd, &pSampler);

    // Initialize cached render states
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    hr = SwapchainHook::d3d11Device->CreateDepthStencilState(&dsd, &pDepthStencilState);

    D3D11_BLEND_DESC bd{};
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = true;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = SwapchainHook::d3d11Device->CreateBlendState(&bd, &pBlendState);

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    hr = SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);

    // Pre-allocate intermediate textures with common screen resolution to reduce first-frame lag
    EnsureIntermediateTextures(MC::windowSize.x, MC::windowSize.y);

}

void DepthOfFieldHelper::RenderToRTV(ID3D11RenderTargetView *pRenderTargetView, ID3D11ShaderResourceView *pShaderResourceView, XMFLOAT2 rtvSize)
{
    ID3D11DeviceContext* pContext = SwapchainHook::context;
    if (!pContext) return;

    // Use cached render states (no recreation overhead)
    pContext->OMSetDepthStencilState(pDepthStencilState, 0);
    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    pContext->RSSetState(pRasterizerState);

    // Clear previous shader resource binding and set render target
    ID3D11ShaderResourceView* nullSRV = nullptr;
    pContext->PSSetShaderResources(0, 1, &nullSRV);
    pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

    // Update constant buffer with current resolution
    constantBuffer.resolution = rtvSize;
    constantBuffer.halfpixel = XMFLOAT2(0.5f / rtvSize.x, 0.5f / rtvSize.y);
    pContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);

    // Set input layout and vertex buffer
    pContext->IASetInputLayout(pInputLayout);
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    // Set shaders and resources
    pContext->VSSetShader(pVertexShader, nullptr, 0);
    pContext->PSSetSamplers(0, 1, &pSampler);
    pContext->PSSetSamplers(1, 1, &pSampler);  // Use same sampler for depth
    pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
    pContext->PSSetShaderResources(0, 1, &pShaderResourceView);

    // Bind depth texture if available
    if (pDepthMapSRV) {
        pContext->PSSetShaderResources(1, 1, &pDepthMapSRV);
    }

    // Set viewport
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = rtvSize.x;
    viewport.Height = rtvSize.y;
    viewport.MaxDepth = 1.0f;
    pContext->RSSetViewports(1, &viewport);

    // Clear and draw
    FLOAT backgroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    pContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);
    pContext->Draw(sizeof(quadVertices) / sizeof(quadVertices[0]), 0);

    // Clear render target binding
    ID3D11RenderTargetView* nullRTV = nullptr;
    pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
}

bool DepthOfFieldHelper::EnsureIntermediateTextures(UINT width, UINT height)
{
    // Fast path: if textures exist and size matches, return immediately
    if (currentTextureWidth == width && currentTextureHeight == height &&
        pIntermediateTexture1 && pIntermediateTexture2 &&
        pIntermediateRTV1 && pIntermediateRTV2 &&
        pIntermediateSRV1 && pIntermediateSRV2) {
        return true;
    }

    // Release existing textures
    ReleaseIntermediateTextures();

    // Create new textures with correct dimensions
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HRESULT hr1 = SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &pIntermediateTexture1);
    HRESULT hr2 = SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &pIntermediateTexture2);

    if (FAILED(hr1) || FAILED(hr2)) {
        ReleaseIntermediateTextures();
        return false;
    }

    // Create render target views
    hr1 = SwapchainHook::d3d11Device->CreateRenderTargetView(pIntermediateTexture1, nullptr, &pIntermediateRTV1);
    hr2 = SwapchainHook::d3d11Device->CreateRenderTargetView(pIntermediateTexture2, nullptr, &pIntermediateRTV2);

    if (FAILED(hr1) || FAILED(hr2)) {
        ReleaseIntermediateTextures();
        return false;
    }

    // Create shader resource views
    hr1 = SwapchainHook::d3d11Device->CreateShaderResourceView(pIntermediateTexture1, nullptr, &pIntermediateSRV1);
    hr2 = SwapchainHook::d3d11Device->CreateShaderResourceView(pIntermediateTexture2, nullptr, &pIntermediateSRV2);

    if (FAILED(hr1) || FAILED(hr2)) {
        ReleaseIntermediateTextures();
        return false;
    }

    currentTextureWidth = width;
    currentTextureHeight = height;

    return true;
}

void DepthOfFieldHelper::ReleaseIntermediateTextures()
{
    if (pIntermediateSRV1) { pIntermediateSRV1->Release(); pIntermediateSRV1 = nullptr; }
    if (pIntermediateSRV2) { pIntermediateSRV2->Release(); pIntermediateSRV2 = nullptr; }
    if (pIntermediateRTV1) { pIntermediateRTV1->Release(); pIntermediateRTV1 = nullptr; }
    if (pIntermediateRTV2) { pIntermediateRTV2->Release(); pIntermediateRTV2 = nullptr; }
    if (pIntermediateTexture1) { pIntermediateTexture1->Release(); pIntermediateTexture1 = nullptr; }
    if (pIntermediateTexture2) { pIntermediateTexture2->Release(); pIntermediateTexture2 = nullptr; }
    currentTextureWidth = 0;
    currentTextureHeight = 0;
}

void DepthOfFieldHelper::Cleanup()
{
    ReleaseIntermediateTextures();

    if (pDepthStencilState) { pDepthStencilState->Release(); pDepthStencilState = nullptr; }
    if (pBlendState) { pBlendState->Release(); pBlendState = nullptr; }
    if (pRasterizerState) { pRasterizerState->Release(); pRasterizerState = nullptr; }
    if (pGaussianBlurHorizontalShader) { pGaussianBlurHorizontalShader->Release(); pGaussianBlurHorizontalShader = nullptr; }
    if (pGaussianBlurVerticalShader) { pGaussianBlurVerticalShader->Release(); pGaussianBlurVerticalShader = nullptr; }
    if (pVertexShader) { pVertexShader->Release(); pVertexShader = nullptr; }
    if (pInputLayout) { pInputLayout->Release(); pInputLayout = nullptr; }
    if (pSampler) { pSampler->Release(); pSampler = nullptr; }
    if (pVertexBuffer) { pVertexBuffer->Release(); pVertexBuffer = nullptr; }
    if (pConstantBuffer) { pConstantBuffer->Release(); pConstantBuffer = nullptr; }
    if (pDepthMapSRV) { pDepthMapSRV->Release(); pDepthMapSRV = nullptr; }

}

void DepthOfFieldHelper::RenderDepthOfField(ID3D11RenderTargetView *pDstRenderTargetView, int iterations, float intensity, float focusRange, float focusDistance, bool autoFocus)
{
    if(intensity <= 0) return;

    if (!SwapchainHook::GetBackbuffer()) return;

    winrt::com_ptr<ID3D11ShaderResourceView> pOrigShaderResourceView = MotionBlur::BackbufferToSRVExtraMode();
    if (!pOrigShaderResourceView) {
        return;
    }

    ID3D11DeviceContext* pContext = SwapchainHook::context;
    if (!pContext) return;

    D3D11_TEXTURE2D_DESC desc;
    SwapchainHook::GetBackbuffer()->GetDesc(&desc);

    // Ensure intermediate textures are available and correct size
    if (!EnsureIntermediateTextures(desc.Width, desc.Height)) {
        return;
    }

    XMFLOAT2 renderSize = XMFLOAT2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));

    // Setup depth of field parameters
    constantBuffer.intensity = intensity;
    constantBuffer.focusRange = focusRange;
    constantBuffer.focusDistance = focusDistance;
    constantBuffer.autoFocus = autoFocus ? 1.0f : 0.0f;
    constantBuffer.offset = XMFLOAT2(1.0f, 1.0f);

    // Use iterations parameter
    int actualIterations = std::max(1, iterations);

    ID3D11ShaderResourceView* currentSRV = pOrigShaderResourceView.get();

    for (int i = 0; i < actualIterations; i++)
    {
        // Clear shader resource binding to avoid conflicts
        ID3D11ShaderResourceView* nullSRV = nullptr;
        pContext->PSSetShaderResources(0, 1, &nullSRV);

        // Horizontal pass
        pContext->PSSetShader(pGaussianBlurHorizontalShader, nullptr, 0);
        RenderToRTV(pIntermediateRTV1, currentSRV, renderSize);

        // Clear binding again
        pContext->PSSetShaderResources(0, 1, &nullSRV);

        // Vertical pass
        pContext->PSSetShader(pGaussianBlurVerticalShader, nullptr, 0);
        if (i == actualIterations - 1) {
            // Last iteration: render to final destination
            RenderToRTV(pDstRenderTargetView, pIntermediateSRV1, renderSize);
        } else {
            // Intermediate iteration: render to second buffer for next iteration
            RenderToRTV(pIntermediateRTV2, pIntermediateSRV1, renderSize);
            currentSRV = pIntermediateSRV2;
        }
    }

}