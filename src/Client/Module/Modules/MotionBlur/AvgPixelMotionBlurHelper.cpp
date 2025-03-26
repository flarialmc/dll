#include "AvgPixelMotionBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
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
    ID3D11Device* m_device = SwapchainHook::d3d11Device;
    if (!m_device) { Logger::debug("Device is nullptr"); return false;}
    Logger::debug("ye");
    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
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
    hr = m_device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
        return false;
    if (!CompileShader(drawTextureShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);
    psBlob->Release();
    if (FAILED(hr))
        return false;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = 16;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
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
    hr = m_device->CreateBuffer(&vbDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr))
        return false;
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
    ID3D11DeviceContext* context = SwapchainHook::context;
    ID3D11Device* device = SwapchainHook::d3d11Device;
    if (!context || !device || !rtv) {
        return;
    }

    // Get size from the provided RTV
    ID3D11Resource* resource = nullptr;
    rtv->GetResource(&resource);
    ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    resource->Release();

    // Set the viewport
    D3D11_VIEWPORT viewport = {0};
    viewport.Width = static_cast<float>(desc.Width);
    viewport.Height = static_cast<float>(desc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    // Set the render target
    context->OMSetRenderTargets(1, &rtv, nullptr);

    const size_t numFrames = frames.size();
    if (numFrames == 0) return;

    // Update constant buffer with number of frames
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        FrameCountBuffer* pData = (FrameCountBuffer*)mappedResource.pData;
        pData->numFrames = (int)numFrames;
        context->Unmap(m_constantBuffer, 0);
    }

    // Set up pipeline
    context->IASetInputLayout(m_inputLayout);
    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_constantBuffer);

    // Bind frames as shader resources
    std::vector<ID3D11ShaderResourceView*> srvs;
    srvs.reserve(numFrames);
    for (auto& frame : frames) {
        srvs.push_back(frame.get());
    }
    context->PSSetShaderResources(0, (UINT)numFrames, srvs.data());

    // Set sampler
    ID3D11SamplerState* sampler = nullptr;
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    HRESULT hr = device->CreateSamplerState(&sampDesc, &sampler);
    if (SUCCEEDED(hr)) {
        context->PSSetSamplers(0, 1, &sampler);
    }

    // Draw
    context->Draw(4, 0);

    std::vector<ID3D11ShaderResourceView*> nullSRVs(numFrames, nullptr);
    context->PSSetShaderResources(0, static_cast<UINT>(numFrames), nullSRVs.data());

    if (sampler) sampler->Release();
}