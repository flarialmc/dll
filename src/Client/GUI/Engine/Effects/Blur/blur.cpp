#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../../../Module/Modules/MotionBlur/MotionBlurListener.hpp"

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
    float intensity;
    float2 resolution;
};

sampler sampler0 : register(s0);
Texture2D inputTexture : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 texSize = resolution;
    float2 texelSize = 1.0f / texSize;
    float2 texCoord = screenSpace.xy / texSize;

    // Scale offsets with a larger factor for more noticeable blur
    float2 offsets[9] = {
        float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f),
        float2(-1.0f,  0.0f), float2(0.0f,  0.0f), float2(1.0f,  0.0f),
        float2(-1.0f,  1.0f), float2(0.0f,  1.0f), float2(1.0f,  1.0f)
    };

    float weights[9];
    float sum = 0.0f;
    float sigma = intensity * 2.0f; // Scale intensity for more noticeable blur
    for (int i = 0; i < 9; i++)
    {
        float2 offset = offsets[i] * texelSize * sigma;
        weights[i] = exp(-dot(offset, offset) / (2.0f * sigma * sigma));
        sum += weights[i];
    }

    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 9; i++)
    {
        float2 offset = offsets[i] * texelSize * sigma;
        color += inputTexture.Sample(sampler0, texCoord + offset) * weights[i];
    }

    color /= sum;

    return color;
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
    ID3DBlob *shaderBlob = nullptr;
    ID3DBlob *errorBlob = nullptr;

    hr = D3DCompile(pSrcData, strlen(pSrcData), nullptr, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::error(std::format("[Blur] Failed to compile shader: {}", static_cast<char*>(errorBlob->GetBufferPointer())));
            errorBlob->Release();
        }
        throw std::logic_error("Failed to compile shader!");
    }

    return shaderBlob;
}


ID3D11PixelShader *dbgShader;

void Blur::InitializePipeline()
{

    HRESULT hr;
    ID3D11DeviceContext* pContext;
    SwapchainHook::d3d11Device->GetImmediateContext(&pContext);

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

    shaderBlob = TryCompileShader(dbgDrawTextureShaderSrc, "ps_4_0");
    SwapchainHook::d3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &dbgShader);

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

    pContext->Release();

}
void Blur::RenderToRTV(ID3D11RenderTargetView *pRenderTargetView, ID3D11ShaderResourceView *pShaderResourceView, XMFLOAT2 rtvSize)
{
    HRESULT hr;
    ID3D11DeviceContext* pContext;
    SwapchainHook::d3d11Device->GetImmediateContext(&pContext);
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    ID3D11DepthStencilState *pDepthStencilState;
    hr = SwapchainHook::d3d11Device->CreateDepthStencilState(&dsd, &pDepthStencilState);
    if (FAILED(hr)) { pContext->Release(); return; }
    pContext->OMSetDepthStencilState(pDepthStencilState, 0);

    void *null = nullptr;
    pContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView **)&null);
    pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

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
    if (FAILED(hr)) { pContext->Release(); pDepthStencilState->Release(); return; }
    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    ID3D11RasterizerState *pRasterizerState;
    hr = SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);
    if (FAILED(hr)) { pContext->Release(); pDepthStencilState->Release(); pBlendState->Release(); return; }
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
    pContext->Release();
}

void Blur::RenderBlur(ID3D11RenderTargetView *pDstRenderTargetView, int iterations, float intensity)
{
    if(intensity < 1) return;

    if (!SwapchainHook::GetBackbuffer()) return;

    ID3D11ShaderResourceView *pOrigShaderResourceView = MotionBlurListener::BackbufferToSRV();
    if (!pOrigShaderResourceView) return;

    ID3D11DeviceContext* pContext;
    SwapchainHook::d3d11Device->GetImmediateContext(&pContext);

    std::vector<ID3D11Texture2D *> framebuffers;
    std::vector<ID3D11RenderTargetView *> renderTargetViews;
    std::vector<ID3D11ShaderResourceView *> shaderResourceViews;
    std::vector<XMFLOAT2> fbSizes;
    D3D11_TEXTURE2D_DESC desc;
    SwapchainHook::GetBackbuffer()->GetDesc(&desc);

    framebuffers.reserve((size_t)iterations);
    renderTargetViews.reserve((size_t)iterations);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.Format = desc.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = 1;

    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

    XMFLOAT2 fbSize = XMFLOAT2(desc.Width, desc.Height);

    constantBuffer.intensity = intensity;
    constantBuffer.resolution = fbSize;

    pContext->PSSetShader(pUpsampleShader, nullptr, 0);
    RenderToRTV(pDstRenderTargetView, pOrigShaderResourceView, fbSize);

    pContext->Release();
    pOrigShaderResourceView->Release();
}
