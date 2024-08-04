#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../../../Module/Modules/MotionBlur/MotionBlurListener.hpp"

// CREDITS @MR CHIPS (@chyves)

#define BLUR_OFFSET 5.0f

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
const char *downsampleShaderSrc = "cbuffer BlurInputBuffer : register(b0)\
{\
    float2 resolution;\
    float2 offset;\
    float2 halfpixel;\
};\
sampler sampler0 : register(s0);\
Texture2D texture0 : register(t0);\
\
float4 main(float4 screenSpace : SV_Position) : SV_TARGET {\
    float2 uv = screenSpace.xy / resolution;\
    float4 sum = texture0.Sample(sampler0, uv) * 4.0;\
    sum += texture0.Sample(sampler0, uv - halfpixel * offset);\
    sum += texture0.Sample(sampler0, uv + halfpixel * offset);\
    sum += texture0.Sample(sampler0, uv + float2(halfpixel.x, -halfpixel.y) * offset);\
    sum += texture0.Sample(sampler0, uv - float2(halfpixel.x, -halfpixel.y) * offset);\
    return sum / 8.0;\
}";
const char *upsampleShaderSrc = "cbuffer BlurInputBuffer : register(b0)\
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
    float4 sum = texture0.Sample(sampler0, uv + float2(-halfpixel.x * 2.0, 0.0) * offset);\
    sum += texture0.Sample(sampler0, uv + float2(-halfpixel.x, halfpixel.y) * offset) * 2.0;\
    sum += texture0.Sample(sampler0, uv + float2(0.0, halfpixel.y * 2.0) * offset);\
    sum += texture0.Sample(sampler0, uv + float2(halfpixel.x, halfpixel.y) * offset) * 2.0;\
    sum += texture0.Sample(sampler0, uv + float2(halfpixel.x * 2.0, 0.0) * offset);\
    sum += texture0.Sample(sampler0, uv + float2(halfpixel.x, -halfpixel.y) * offset) * 2.0;\
    sum += texture0.Sample(sampler0, uv + float2(0.0, -halfpixel.y * 2.0) * offset);\
    sum += texture0.Sample(sampler0, uv + float2(-halfpixel.x, -halfpixel.y) * offset) * 2.0;\
    return sum / 12.0;\
}";
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
        Logger::error("[Blur] Failed to compile shader");
        errorBlob->Release();
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
    SwapchainHook::d3d11Device->CreateDepthStencilState(&dsd, &pDepthStencilState);
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
    D3D11_BLEND_DESC bd;
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
    SwapchainHook::d3d11Device->CreateBlendState(&bd, &pBlendState);
    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    ID3D11RasterizerState *pRasterizerState;
    SwapchainHook::d3d11Device->CreateRasterizerState(&rd, &pRasterizerState);
    pContext->RSSetState(pRasterizerState);

    pContext->PSSetShaderResources(0, 1, &pShaderResourceView);
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = rtvSize.x;
    viewport.Height = rtvSize.y;
    viewport.MaxDepth = 1.0f;

    FLOAT backgroundColor[4] = {0.1f, 0.2f, 0.6f, 1.0f};
    pContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);
    pContext->RSSetViewports(1, &viewport);
    pContext->Draw(sizeof(quadVertices) / sizeof(quadVertices[0]), 0);
    ID3D11RenderTargetView* kajgd = nullptr;
    pContext->OMSetRenderTargets(1, &kajgd, nullptr);
}

void Blur::RenderBlur(ID3D11RenderTargetView *pDstRenderTargetView, int iterations)
{

    ID3D11Texture2D* tex = MotionBlurListener::GetBackbuffer();
    if(!tex) return;

    ID3D11ShaderResourceView *pOrigShaderResourceView = MotionBlurListener::SaveBackbuffer();
    if(!pOrigShaderResourceView) return;

    ID3D11DeviceContext* pContext;
    SwapchainHook::d3d11Device->GetImmediateContext(&pContext);

    HRESULT hr;
    std::vector<ID3D11Texture2D *> framebuffers;
    std::vector<ID3D11RenderTargetView *> renderTargetViews;
    std::vector<ID3D11ShaderResourceView *> shaderResourceViews;
    std::vector<XMFLOAT2> fbSizes;
    D3D11_TEXTURE2D_DESC desc;
    tex->GetDesc(&desc);

    framebuffers.reserve((size_t)iterations);
    renderTargetViews.reserve((size_t)iterations);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.Format = desc.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = 1;


    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

    // ! I should probably reuse the textures and RenderTargetViews
    for (int i = 0; i <= iterations; i++)
    {
        ID3D11Texture2D *pFrameBuffer;
        ID3D11RenderTargetView *pRenderTargetView;
        ID3D11ShaderResourceView *pShaderResourceView;

        SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &pFrameBuffer);
        if (i == 0)
            pRenderTargetView = pDstRenderTargetView;
        else
            SwapchainHook::d3d11Device->CreateRenderTargetView(pFrameBuffer, nullptr, &pRenderTargetView);
        SwapchainHook::d3d11Device->CreateShaderResourceView(pFrameBuffer, nullptr, &pShaderResourceView);

        framebuffers.push_back(pFrameBuffer);
        renderTargetViews.push_back(pRenderTargetView);
        shaderResourceViews.push_back(pShaderResourceView);
        fbSizes.push_back(XMFLOAT2(desc.Width, desc.Height));

        desc.Width /= 2;
        desc.Height /= 2;
    }


    constantBuffer.offset = XMFLOAT2(BLUR_OFFSET, BLUR_OFFSET);
    pContext->PSSetShader(pDownsampleShader, nullptr, 0);
    RenderToRTV(renderTargetViews[1], pOrigShaderResourceView, fbSizes[1]);

    for (int i = 1; i < iterations; i++)
    {
        RenderToRTV(renderTargetViews[i + 1], shaderResourceViews[i], fbSizes[i + 1]);
    }

    pContext->PSSetShader(pUpsampleShader, nullptr, 0);

    for (int i = iterations; i > 0; i--)
    {
        if (renderTargetViews[i - 1] == pDstRenderTargetView)
        RenderToRTV(renderTargetViews[i - 1], shaderResourceViews[i], fbSizes[i - 1]);
    }

    // pContext->PSSetShader(dbgShader, nullptr, 0);
    // RenderToRTV(pDstRenderTargetView, shaderResourceViews[0], fbSizes[0]);

    for (int i = 0; i < iterations; i++)
    {
        if (i != 0)
            renderTargetViews[i]->Release();
        framebuffers[i]->Release();
        shaderResourceViews[i]->Release();
    }

    tex->Release();
    pOrigShaderResourceView->Release();
}