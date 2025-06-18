#include "AvgPixelMotionBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <unknwn.h>
#include <assert.h>

#include "Hook/Hooks/Render/SwapchainHook.hpp"


const char* drawTextureShaderSrc = R"(
cbuffer FrameCountBuffer : register(b0)
{
    int numFrames;
    float3 padding;
};
Texture2D g_frames[50] : register(t0);
SamplerState g_sampler : register(s0);
struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);
    int safeNumFrames = max(numFrames, 1);
    [unroll]
    for (int i = 0; i < safeNumFrames; i++)
    {
        color += g_frames[i].Sample(g_sampler, input.Tex);
    }
    return color / safeNumFrames;
}

)";

const char* drawTextureVertexShaderSrc = R"(
struct VS_INPUT {
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};
struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = float4(input.Pos, 1.0);
    output.Tex = input.Tex;
    return output;
}
)";

struct FrameCountBuffer {
    int numFrames;
    float padding[3];
};

bool AvgPixelMotionBlurHelper::Initialize()
{
    Logger::debug("initializing motion blur helper..");
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    Logger::debug("ye");

    if (!CompileShader(drawTextureVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;
    ID3D11Device* m_device = SwapchainHook::d3d11Device.get();
    if (!m_device) { Logger::debug("Device is nullptr"); return false;}
    Logger::debug("ye");
    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.put());
    if (FAILED(hr))
    {
        vsBlob->Release();
        return false;
    }
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                           D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(float)*3,             D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = m_device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.put());
    vsBlob->Release();
    if (FAILED(hr))
        return false;
    if (!CompileShader(drawTextureShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.put());
    psBlob->Release();
    if (FAILED(hr))
        return false;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = 16;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.put());
    if (FAILED(hr))
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to create constant buffer: 0x%08X\n", hr);
        std::cout << "umm what the sigma" << std::endl;
        OutputDebugStringA(errorMsg);
        return false;
    }
    struct Vertex { float x, y, z; float u, v; };
    Vertex vertices[] =
    {
        { -1.0f,  1.0f, 0.0f,     0.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f,     1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f,     0.0f, 1.0f },
        {  1.0f, -1.0f, 0.0f,     1.0f, 1.0f },
    };
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = m_device->CreateBuffer(&vbDesc, &initData, m_vertexBuffer.put());
    if (FAILED(hr))
        return false;

    // Create cached render states
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    hr = m_device->CreateDepthStencilState(&dsd, m_depthStencilState.put());
    if (FAILED(hr))
        return false;

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
    hr = m_device->CreateBlendState(&bd, m_blendState.put());
    if (FAILED(hr))
        return false;

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    hr = m_device->CreateRasterizerState(&rd, m_rasterizerState.put());
    if (FAILED(hr))
        return false;

    // Create cached sampler state
    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = m_device->CreateSamplerState(&sampDesc, m_samplerState.put());
    if (FAILED(hr))
        return false;

    // Initialize cached viewport to zero to ensure first update
    memset(&m_cachedViewport, 0, sizeof(m_cachedViewport));

    return true;
}

bool AvgPixelMotionBlurHelper::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    UINT compileFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(srcData, strlen(srcData), nullptr, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::debug((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    if (errorBlob) errorBlob->Release();
    return true;
}

void AvgPixelMotionBlurHelper::Render(ID3D11RenderTargetView* rtv, std::vector<winrt::com_ptr<ID3D11ShaderResourceView>>& frames)
{
    ID3D11DeviceContext* context = SwapchainHook::context.get();
    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!context || !device || !rtv) {
        return;
    }

    ID3D11RenderTargetView* originalRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    UINT numRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    ID3D11DepthStencilView* originalDepthStencilView = nullptr;
    context->OMGetRenderTargets(numRenderTargets, originalRenderTargetViews, &originalDepthStencilView);


    // Only update viewport if window size changed
    float currentWidth = static_cast<float>(MC::windowSize.x);
    float currentHeight = static_cast<float>(MC::windowSize.y);
    if (m_cachedViewport.Width != currentWidth || m_cachedViewport.Height != currentHeight) {
        m_cachedViewport.TopLeftX = 0;
        m_cachedViewport.TopLeftY = 0;
        m_cachedViewport.Width = currentWidth;
        m_cachedViewport.Height = currentHeight;
        m_cachedViewport.MinDepth = 0.0f;
        m_cachedViewport.MaxDepth = 1.0f;
    }
    context->RSSetViewports(1, &m_cachedViewport);

    context->ClearRenderTargetView(rtv, BACKGROUND_COLOR);

    context->OMSetRenderTargets(1, &rtv, originalDepthStencilView);


    context->OMSetDepthStencilState(m_depthStencilState.get(), 0);
    context->OMSetBlendState(m_blendState.get(), nullptr, 0xffffffff);
    context->RSSetState(m_rasterizerState.get());

    // -------------------------------
    // Update constant buffer with number of frames
    // -------------------------------
    const size_t numFrames = frames.size();
    if (numFrames == 0) {
        return;
    }
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        FrameCountBuffer* pData = (FrameCountBuffer*)mappedResource.pData;
        pData->numFrames = static_cast<int>(numFrames);
        context->Unmap(m_constantBuffer.get(), 0);
    }

    // -------------------------------
    // Set up pipeline states
    // -------------------------------
    context->IASetInputLayout(m_inputLayout.get());
    ID3D11Buffer* vertexBuffer = m_vertexBuffer.get();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &VERTEX_STRIDE, &VERTEX_OFFSET);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    ID3D11Buffer* constantBuffer = m_constantBuffer.get();
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    // -------------------------------
    // Bind frames as shader resources
    // -------------------------------
    // Use cached vector to avoid dynamic allocation
    m_srvCache.clear();
    m_srvCache.reserve(numFrames);
    for (auto& frame : frames) {
        m_srvCache.push_back(frame.get());
    }
    context->PSSetShaderResources(0, static_cast<UINT>(numFrames), m_srvCache.data());

    // -------------------------------
    // Set sampler state
    // -------------------------------

    ID3D11SamplerState* samplerPtr = m_samplerState.get();
    context->PSSetSamplers(0, 1, &samplerPtr);

    // -------------------------------
    // Draw
    // -------------------------------
    context->Draw(4, 0);

    // Unbind shader resources using cached vector
    m_nullSRVCache.clear();
    m_nullSRVCache.resize(numFrames, nullptr);
    context->PSSetShaderResources(0, static_cast<UINT>(numFrames), m_nullSRVCache.data());

    if (originalDepthStencilView) originalDepthStencilView->Release();
    for (UINT i = 0; i < numRenderTargets; ++i) {
        if (originalRenderTargetViews[i]) originalRenderTargetViews[i]->Release();
    }
}
