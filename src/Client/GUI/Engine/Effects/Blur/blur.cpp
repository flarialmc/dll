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

const char *downsampleShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);

    static const float2 offsets[9] = {
        float2(-1.0, -1.0) * halfPixel * offset,
        float2(0.0, -1.0) * halfPixel * offset,
        float2(1.0, -1.0) * halfPixel * offset,
        float2(-1.0, 0.0) * halfPixel * offset,
        float2(0.0, 0.0) * halfPixel * offset,
        float2(1.0, 0.0) * halfPixel * offset,
        float2(-1.0, 1.0) * halfPixel * offset,
        float2(0.0, 1.0) * halfPixel * offset,
        float2(1.0, 1.0) * halfPixel * offset
    };

    static const float weights[9] = {
        0.06136, 0.12245, 0.06136,
        0.12245, 0.24477, 0.12245,
        0.06136, 0.12245, 0.06136
    };

    float weightSum = 0.0;
    for (int i = 0; i < 9; i++)
    {
        weightSum += weights[i];
    }

    for (int i = 0; i < 9; i++)
    {
        colorSum += texture0.Sample(sampler0, uv + offsets[i]) * (weights[i] / weightSum);
    }

    return colorSum;
}
)";

const char *upsampleShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);

    static const float2 offsets[9] = {
        float2(-1.0, -1.0) * halfPixel * offset,
        float2(0.0, -1.0) * halfPixel * offset,
        float2(1.0, -1.0) * halfPixel * offset,
        float2(-1.0, 0.0) * halfPixel * offset,
        float2(0.0, 0.0) * halfPixel * offset,
        float2(1.0, 0.0) * halfPixel * offset,
        float2(-1.0, 1.0) * halfPixel * offset,
        float2(0.0, 1.0) * halfPixel * offset,
        float2(1.0, 1.0) * halfPixel * offset
    };

    static const float weights[9] = {
        0.06136, 0.12245, 0.06136,
        0.12245, 0.24477, 0.12245,
        0.06136, 0.12245, 0.06136
    };

    float weightSum = 0.0;
    for (int i = 0; i < 9; i++)
    {
        weightSum += weights[i];
    }

    for (int i = 0; i < 9; i++)
    {
        colorSum += texture0.Sample(sampler0, uv + offsets[i]) * (weights[i] / weightSum);
    }

    return colorSum;
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

    ID3DBlob *shaderBlob = TryCompileShader(upsampleShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pUpsampleShader);

    shaderBlob = TryCompileShader(downsampleShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pDownsampleShader);

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
    
}
void Blur::RenderToRTV(ID3D11RenderTargetView *pRenderTargetView, ID3D11ShaderResourceView *pShaderResourceView, XMFLOAT2 rtvSize)
{
    HRESULT hr;
    ID3D11DeviceContext* pContext = SwapchainHook::context;
    if (!pContext) return;
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    ID3D11DepthStencilState *pDepthStencilState;
    hr = SwapchainHook::d3d11Device->CreateDepthStencilState(&dsd, &pDepthStencilState);
    if (FAILED(hr)) {  return; }
    pContext->OMSetDepthStencilState(pDepthStencilState, 0);

    void *null = nullptr;
    pContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView **)&null);
    pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

    constantBuffer.resolution = rtvSize;
    constantBuffer.halfpixel = XMFLOAT2(0.5 / rtvSize.x, 0.5 / rtvSize.y);
    pContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);

    pContext->IASetInputLayout(pInputLayout);
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;

    pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    pContext->VSSetShader(pVertexShader, nullptr, 0);
    pContext->PSSetSamplers(0, 1, &pSampler);
    pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
    D3D11_BLEND_DESC bd{};
    ZeroMemory(&bd, sizeof(bd));
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = true;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    ID3D11BlendState *pBlendState;
    hr = SwapchainHook::d3d11Device->CreateBlendState(&bd, &pBlendState);
    if (FAILED(hr)) {  pDepthStencilState->Release(); return; }
    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    ID3D11RasterizerState *pRasterizerState;
    hr = SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);
    if (FAILED(hr)) {  pDepthStencilState->Release(); pBlendState->Release(); return; }
    pContext->RSSetState(pRasterizerState);

    pContext->PSSetShaderResources(0, 1, &pShaderResourceView);
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = rtvSize.x;
    viewport.Height = rtvSize.y;
    viewport.MaxDepth = 1.0f;

    FLOAT backgroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    pContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);
    pContext->RSSetViewports(1, &viewport);
    pContext->Draw(sizeof(quadVertices) / sizeof(quadVertices[0]), 0);
    ID3D11RenderTargetView* kajgd = nullptr;
    pContext->OMSetRenderTargets(1, &kajgd, nullptr);

    pDepthStencilState->Release();
    pBlendState->Release();
    pRasterizerState->Release();
}

void Blur::RenderBlur(ID3D11RenderTargetView *pDstRenderTargetView, int iterations, float intensity)
{

    if(intensity < 0) return;

    if (!SwapchainHook::GetBackbuffer()) return;

    winrt::com_ptr<ID3D11ShaderResourceView> pOrigShaderResourceView = MotionBlur::BackbufferToSRVExtraMode();
    if (!pOrigShaderResourceView) {
        return;
    }

    ID3D11DeviceContext* pContext = SwapchainHook::context;
    if (!pContext) return;

    std::vector<ID3D11RenderTargetView *> renderTargetViews;
    std::vector<ID3D11ShaderResourceView *> shaderResourceViews;
    std::vector<XMFLOAT2> fbSizes;
    D3D11_TEXTURE2D_DESC desc;
    SwapchainHook::GetBackbuffer()->GetDesc(&desc);

    if(!hasDoneFrames)
    framebuffers.reserve((size_t)iterations);
    renderTargetViews.reserve((size_t)iterations);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.Format = desc.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = 1;

    desc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    for (int i = 0; i <= iterations; i++)
    {
        ID3D11Texture2D *pFrameBuffer;
        ID3D11RenderTargetView *pRenderTargetView;
        ID3D11ShaderResourceView *pShaderResourceView;


        // create texture2d for each size and simply reuse to create rtvs & srvs
        if(!hasDoneFrames)
        SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &pFrameBuffer);
        else pFrameBuffer = framebuffers[i];
        if (i == 0)
            pRenderTargetView = pDstRenderTargetView;
        else
            SwapchainHook::d3d11Device->CreateRenderTargetView(pFrameBuffer, nullptr, &pRenderTargetView);
        SwapchainHook::d3d11Device->CreateShaderResourceView(pFrameBuffer, nullptr, &pShaderResourceView);

        if(!hasDoneFrames)
        framebuffers.push_back(pFrameBuffer);
        renderTargetViews.push_back(pRenderTargetView);
        shaderResourceViews.push_back(pShaderResourceView);
        fbSizes.push_back(XMFLOAT2(desc.Width, desc.Height));

        desc.Width /= 2;
        desc.Height /= 2;
    }

    hasDoneFrames = true;

    constantBuffer.offset = XMFLOAT2(intensity * 3, intensity * 3);
    pContext->PSSetShader(pDownsampleShader, nullptr, 0);
    RenderToRTV(renderTargetViews[1], pOrigShaderResourceView.get(), fbSizes[1]);

    for (int i = 1; i < iterations; i++)
    {
        RenderToRTV(renderTargetViews[i + 1], shaderResourceViews[i], fbSizes[i + 1]);
    }

    pContext->PSSetShader(pUpsampleShader, nullptr, 0);

    for (int i = iterations; i > 0; i--)
    {
        RenderToRTV(renderTargetViews[i - 1], shaderResourceViews[i], fbSizes[i - 1]);
    }

    for (int i = 0; i < iterations; i++)
    {

        if (i != 0)
            renderTargetViews[i]->Release();
        shaderResourceViews[i]->Release();

        renderTargetViews.clear();
        shaderResourceViews.clear();
        fbSizes.clear();
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