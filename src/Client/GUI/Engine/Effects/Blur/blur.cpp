#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Client.hpp"
#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../../../Module/Modules/MotionBlur/MotionBlur.hpp"

// CREDITS @MR CHIPS (@chyves)

#define BLUR_OFFSET 10

static const XMFLOAT4 quadVertices[] =
    {
        XMFLOAT4(1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, -1.0, 0.0, 1.0)};

const char *vertexShaderSrc = "struct VS_INPUT {\
    float4 pos : POSITION;\
};\
\
float4 main(VS_INPUT input) : SV_POSITION {\
    return input.pos;\
}";

const char *gaussianBlurHorizontalShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
    float intensity;
    float _padding;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    
    // Adaptive sampling based on intensity
    int sampleCount = clamp((int)(intensity * 4.0 + 5.0), 5, 15);
    float weightSum = 0.0;
    
    // Calculate gaussian weights dynamically
    float sigma = intensity * 1.5 + 0.5;
    float twoSigmaSq = 2.0 * sigma * sigma;
    
    int halfSamples = sampleCount / 2;
    
    for (int i = -halfSamples; i <= halfSamples; i++)
    {
        float x = (float)i;
        float weight = exp(-(x * x) / twoSigmaSq);
        float2 sampleOffset = float2(x * halfPixel.x * intensity, 0.0);
        
        colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
        weightSum += weight;
    }
    
    return colorSum / weightSum;
}
)";

const char *gaussianBlurVerticalShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
    float intensity;
    float _padding;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
    
    // Adaptive sampling based on intensity
    int sampleCount = clamp((int)(intensity * 4.0 + 5.0), 5, 15);
    float weightSum = 0.0;
    
    // Calculate gaussian weights dynamically
    float sigma = intensity * 1.5 + 0.5;
    float twoSigmaSq = 2.0 * sigma * sigma;
    
    int halfSamples = sampleCount / 2;
    
    for (int i = -halfSamples; i <= halfSamples; i++)
    {
        float y = (float)i;
        float weight = exp(-(y * y) / twoSigmaSq);
        float2 sampleOffset = float2(0.0, y * halfPixel.y * intensity);
        
        colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
        weightSum += weight;
    }
    
    return colorSum / weightSum;
}
)";



const char *dbgDrawTextureShaderSrc = "cbuffer BlurInputBuffer : register(b0)\
{\
    float2 resolution;\
    float2 offset;\
    float2 halfpixel;\
};\
struct PS_INPUT {\
    float4 pos : POSITION;\
};\
sampler sampler0 : register(s0);\
Texture2D texture0 : register(t0);\
\
float4 main(PS_INPUT input, float4 screenSpace : SV_Position) : SV_TARGET {\
    float2 uv = screenSpace.xy / resolution;\
    return texture0.Sample(sampler0, uv);\
}";

ID3DBlob *TryCompileShader(const char *pSrcData, const char *pTarget)
{
    HRESULT hr;

    ID3DBlob *shaderBlob;
    ID3DBlob *errorBlob;
    hr = D3DCompile(pSrcData, strlen(pSrcData), nullptr, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        LOG_ERROR("[Blur] Failed to compile shader");
        errorBlob->Release();
        throw std::logic_error("Failed to compile shader!");
    }
    return shaderBlob;
}

ID3D11PixelShader *dbgShader;

void Blur::InitializePipeline()
{
    HRESULT hr;
    ID3D11DeviceContext* pContext = SwapchainHook::context;

    // byteWidth has to be a multiple of 32, BlurInputBuffer has a size of 24
    CD3D11_BUFFER_DESC cbd(
        sizeof(BlurInputBuffer),
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

    ID3DBlob *shaderBlob = TryCompileShader(gaussianBlurHorizontalShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pGaussianBlurHorizontalShader);

    shaderBlob = TryCompileShader(gaussianBlurVerticalShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pGaussianBlurVerticalShader);

    shaderBlob = TryCompileShader(vertexShaderSrc, "vs_4_0");
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
void Blur::RenderToRTV(ID3D11RenderTargetView *pRenderTargetView, ID3D11ShaderResourceView *pShaderResourceView, XMFLOAT2 rtvSize)
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
    pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
    pContext->PSSetShaderResources(0, 1, &pShaderResourceView);
    
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

bool Blur::EnsureIntermediateTextures(UINT width, UINT height)
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

void Blur::ReleaseIntermediateTextures()
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

void Blur::Cleanup()
{
    ReleaseIntermediateTextures();
    
    if (pDepthStencilState) { pDepthStencilState->Release(); pDepthStencilState = nullptr; }
    if (pBlendState) { pBlendState->Release(); pBlendState = nullptr; }
    if (pRasterizerState) { pRasterizerState->Release(); pRasterizerState = nullptr; }
}

void Blur::RenderBlur(ID3D11RenderTargetView *pDstRenderTargetView, int iterations, float intensity)
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

    // Setup intensity and offset for blur
    constantBuffer.intensity = intensity;
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

static std::chrono::steady_clock::time_point frameTimestamp = std::chrono::high_resolution_clock::now();

float calculateTimeBetweenFrames(int fps) {
    if (fps >= 120) {
        return 1.f / 120.f;  // 120 FPS
    } else if (fps >= 60) {
        return 1.f / 60.f;   // 60 FPS
    } else if (fps >= 50) {
        return 1.f / 30.f;   // 30 FPS
    } else if (fps > 35) {
        return 1.f / 20.f;   // 20 FPS
    } else {
        return 1.f / 15.f;   // 15 FPS
    }
}

D2D1_GAUSSIANBLUR_OPTIMIZATION getOptimizationLevel(int fps, bool highQuality, bool dynamic) {
    if(fps > 120 || !dynamic) {
        return highQuality ? D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY : D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    } else if (fps >= 55) {
        return D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    } else {
        return D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED;
    }
}

void FlarialGUI::PrepareBlur(float intensity) {
    // Create Gaussian blur effect
    if (FlarialGUI::blur == nullptr) {

        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);

        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);
    }

    if (SwapchainHook::init) {
        std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - frameTimestamp;

        auto currentBlurIntensity = Client::settings.getSettingByName<float>("blurintensity")->value;
        auto delta = currentBlurIntensity - intensity;

        float timeBetweenFrames = calculateTimeBetweenFrames(MC::fps);

        auto shouldLimitSetting = Client::settings.getSettingByName<bool>("limitblurfps");
        auto highQualityBlurSetting = Client::settings.getSettingByName<bool>("highqualityblur");
        auto dynamicBlurQualitySetting = Client::settings.getSettingByName<bool>("dynamicblurrquality");

        auto shouldLimit = shouldLimitSetting ? shouldLimitSetting->value : true;
        auto highQualityBlur = highQualityBlurSetting ? highQualityBlurSetting->value : false;
        auto dynamicBlurQuality = dynamicBlurQualitySetting ? dynamicBlurQualitySetting->value : true;

        auto shouldUpdate = (shouldLimit && elapsed.count() >= timeBetweenFrames) || !shouldLimit;
        auto isLerping = delta > 0.001 || delta < -0.1;

        if (isLerping || shouldUpdate || FlarialGUI::blur_bitmap_cache) {
            if (SwapchainHook::queue != nullptr)
                FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap],
                                       &FlarialGUI::screen_bitmap_cache);
            else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &FlarialGUI::screen_bitmap_cache);

            FlarialGUI::blur->SetInput(0, FlarialGUI::screen_bitmap_cache);
            FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
            FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION,
                                       getOptimizationLevel(MC::fps, highQualityBlur, dynamicBlurQuality));
            FlarialGUI::blur->GetOutput(&FlarialGUI::blur_bitmap_cache);

            frameTimestamp = std::chrono::high_resolution_clock::now();
        }
    }
}

void FlarialGUI::AllahBlur(float intensity) {
    FlarialGUI::PrepareBlur(intensity);
    if (FlarialGUI::blur_bitmap_cache != nullptr)
        D2D::context->DrawImage(FlarialGUI::blur_bitmap_cache);
}