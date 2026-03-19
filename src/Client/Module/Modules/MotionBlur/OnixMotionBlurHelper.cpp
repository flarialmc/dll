#include "OnixMotionBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <unknwn.h>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

// 5x5 box blur shader for Onix motion blur
const char* onixBoxBlurShaderSrc = R"(
cbuffer BlurParams : register(b0) {
    float2 texelSize;  // 1.0 / texture dimensions
    float2 padding;
};

Texture2D inputTexture : register(t0);
SamplerState samplerState : register(s0);

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 mainPS(VS_OUTPUT input) : SV_Target {
    float4 color = float4(0, 0, 0, 0);
    const int radius = 2;  // 5x5 kernel

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float2 offset = float2(x, y) * texelSize;
            color += inputTexture.Sample(samplerState, input.Tex + offset);
        }
    }

    return color / 25.0;  // Average of 25 samples
}
)";

const char* onixVertexShaderSrc = R"(
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

struct BlurParamsBuffer {
    float texelSize[2];
    float padding[2];
};

bool OnixMotionBlurHelper::Initialize()
{
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (!CompileShader(onixVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;

    ID3D11Device* m_device = SwapchainHook::d3d11Device.get();
    if (!m_device) {
        return false;
    }

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

    if (!CompileShader(onixBoxBlurShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_boxBlurShader.put());
    psBlob->Release();
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(BlurParamsBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.put());
    if (FAILED(hr))
        return false;

    struct Vertex { float x, y, z; float u, v; };
    Vertex vertices[] =
    {
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
        {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
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

    // Create render states
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    hr = m_device->CreateDepthStencilState(&dsd, m_depthStencilState.put());
    if (FAILED(hr))
        return false;

    D3D11_BLEND_DESC bd{};
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = false;
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

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = m_device->CreateSamplerState(&sampDesc, m_samplerState.put());
    if (FAILED(hr))
        return false;

    memset(&m_cachedViewport, 0, sizeof(m_cachedViewport));
    m_cachedWidth = 0;
    m_cachedHeight = 0;

    return true;
}

bool OnixMotionBlurHelper::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
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
            errorBlob->Release();
        return false;
    }
    if (errorBlob)
        errorBlob->Release();
    return true;
}

bool OnixMotionBlurHelper::CreateRenderTargets(int width, int height)
{
    CleanupRenderTargets();

    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!device)
        return false;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_pingTexture.put());
    if (FAILED(hr))
        return false;

    hr = device->CreateTexture2D(&texDesc, nullptr, m_pongTexture.put());
    if (FAILED(hr))
        return false;

    hr = device->CreateRenderTargetView(m_pingTexture.get(), nullptr, m_pingRTV.put());
    if (FAILED(hr))
        return false;

    hr = device->CreateRenderTargetView(m_pongTexture.get(), nullptr, m_pongRTV.put());
    if (FAILED(hr))
        return false;

    hr = device->CreateShaderResourceView(m_pingTexture.get(), nullptr, m_pingSRV.put());
    if (FAILED(hr))
        return false;

    hr = device->CreateShaderResourceView(m_pongTexture.get(), nullptr, m_pongSRV.put());
    if (FAILED(hr))
        return false;

    m_cachedWidth = width;
    m_cachedHeight = height;

    m_cachedViewport.TopLeftX = 0;
    m_cachedViewport.TopLeftY = 0;
    m_cachedViewport.Width = static_cast<float>(width);
    m_cachedViewport.Height = static_cast<float>(height);
    m_cachedViewport.MinDepth = 0.0f;
    m_cachedViewport.MaxDepth = 1.0f;

    return true;
}

void OnixMotionBlurHelper::CleanupRenderTargets()
{
    m_pingSRV = nullptr;
    m_pongSRV = nullptr;
    m_pingRTV = nullptr;
    m_pongRTV = nullptr;
    m_pingTexture = nullptr;
    m_pongTexture = nullptr;
}

void OnixMotionBlurHelper::Reset()
{
    CleanupRenderTargets();
    m_cachedWidth = 0;
    m_cachedHeight = 0;
}

winrt::com_ptr<ID3D11ShaderResourceView> OnixMotionBlurHelper::Render(
    winrt::com_ptr<ID3D11ShaderResourceView>& inputFrame)
{
    ID3D11DeviceContext* context = SwapchainHook::context.get();
    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!context || !device || !inputFrame)
        return nullptr;

    // Get input frame dimensions
    ID3D11Resource* frameResource = nullptr;
    inputFrame->GetResource(&frameResource);
    ID3D11Texture2D* frameTexture = static_cast<ID3D11Texture2D*>(frameResource);
    D3D11_TEXTURE2D_DESC frameDesc;
    frameTexture->GetDesc(&frameDesc);
    frameResource->Release();

    int width = static_cast<int>(frameDesc.Width);
    int height = static_cast<int>(frameDesc.Height);

    // Recreate render targets if size changed
    if (width != m_cachedWidth || height != m_cachedHeight)
    {
        if (!CreateRenderTargets(width, height))
            return nullptr;
    }

    // Copy input frame to ping texture
    context->CopyResource(m_pingTexture.get(), frameTexture);

    // Set up pipeline states
    context->IASetInputLayout(m_inputLayout.get());
    ID3D11Buffer* vertexBuffer = m_vertexBuffer.get();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &VERTEX_STRIDE, &VERTEX_OFFSET);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_boxBlurShader.get(), nullptr, 0);
    context->OMSetDepthStencilState(m_depthStencilState.get(), 0);
    context->OMSetBlendState(m_blendState.get(), nullptr, 0xffffffff);
    context->RSSetState(m_rasterizerState.get());
    context->RSSetViewports(1, &m_cachedViewport);

    // Update constant buffer with texel size
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        BlurParamsBuffer* pData = (BlurParamsBuffer*)mappedResource.pData;
        pData->texelSize[0] = 1.0f / static_cast<float>(width);
        pData->texelSize[1] = 1.0f / static_cast<float>(height);
        pData->padding[0] = 0.0f;
        pData->padding[1] = 0.0f;
        context->Unmap(m_constantBuffer.get(), 0);
    }

    ID3D11Buffer* constantBuffer = m_constantBuffer.get();
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    ID3D11SamplerState* samplerPtr = m_samplerState.get();
    context->PSSetSamplers(0, 1, &samplerPtr);

    // Perform 12-pass iterative blur using ping-pong buffers
    bool usePingAsSource = true;  // Start with ping as source (has input frame)

    for (int pass = 0; pass < NUM_PASSES; ++pass)
    {
        // Set source and destination based on current pass
        ID3D11ShaderResourceView* sourceSRV = usePingAsSource ? m_pingSRV.get() : m_pongSRV.get();
        ID3D11RenderTargetView* destRTV = usePingAsSource ? m_pongRTV.get() : m_pingRTV.get();

        context->OMSetRenderTargets(1, &destRTV, nullptr);
        context->PSSetShaderResources(0, 1, &sourceSRV);

        context->Draw(4, 0);

        // Unbind SRV to avoid read/write hazards
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        context->PSSetShaderResources(0, 1, nullSRV);

        // Swap for next pass
        usePingAsSource = !usePingAsSource;
    }

    // After 12 passes, the result is in pong (since we started with ping and did even number of passes)
    return m_pongSRV;
}
