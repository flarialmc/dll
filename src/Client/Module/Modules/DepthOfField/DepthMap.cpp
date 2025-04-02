#pragma once
#include "DepthMap.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <assert.h>
#include "Hook/Hooks/Render/SwapchainHook.hpp"

// Include stb_image_write for PNG saving
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ImGui/stb_image_write.h>

// Define static member variables
ID3D11VertexShader* DepthMapGenerator::m_vertexShader = nullptr;
ID3D11PixelShader* DepthMapGenerator::m_pixelShader = nullptr;
ID3D11InputLayout* DepthMapGenerator::m_inputLayout = nullptr;
ID3D11Buffer* DepthMapGenerator::m_vertexBuffer = nullptr;
ID3D11RenderTargetView* DepthMapGenerator::m_renderTargetView = nullptr;
ID3D11Texture2D* DepthMapGenerator::m_renderTexture = nullptr;
ID3D11SamplerState* DepthMapGenerator::m_samplerState = nullptr;

// Static shader source strings
static const char* depthMapVertexShaderSrc = R"(
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

static const char* depthMapPixelShaderSrc = R"(
Texture2D<float> g_depthTexture : register(t0);
SamplerState g_sampler : register(s0);
struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float depth = g_depthTexture.Sample(g_sampler, input.Tex);
    float gray = 1.0 - depth;
    return float4(gray, gray, gray, 1.0);
}
)";

bool DepthMapGenerator::Initialize()
{
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // Compile vertex shader
    if (!CompileShader(depthMapVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;

    ID3D11Device* device = SwapchainHook::d3d11Device;
    if (!device) return false;

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(hr))
    {
        vsBlob->Release();
        return false;
    }

    // Define input layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
        return false;

    // Compile pixel shader
    if (!CompileShader(depthMapPixelShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);
    psBlob->Release();
    if (FAILED(hr))
        return false;

    // Create vertex buffer
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
    hr = device->CreateBuffer(&vbDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr))
        return false;

    // Create render texture and render target view
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = static_cast<UINT>(MC::windowSize.x);
    texDesc.Height = static_cast<UINT>(MC::windowSize.y);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    hr = device->CreateTexture2D(&texDesc, nullptr, &m_renderTexture);
    if (FAILED(hr))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateRenderTargetView(m_renderTexture, &rtvDesc, &m_renderTargetView);
    if (FAILED(hr))
    {
        m_renderTexture->Release();
        return false;
    }

    // Create sampler state
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = device->CreateSamplerState(&sampDesc, &m_samplerState);
    if (FAILED(hr))
        return false;

    return true;
}

bool DepthMapGenerator::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(srcData, strlen(srcData), nullptr, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    if (errorBlob) errorBlob->Release();
    return true;
}

bool DepthMapGenerator::GenerateDepthMap(ID3D11Texture2D* depthTexture)
{
    ID3D11Device* device = SwapchainHook::d3d11Device;
    ID3D11DeviceContext* context = SwapchainHook::context;
    if (!device || !context || !depthTexture)
        return false;
    Logger::debug("ERROR {} {} {}", device, context, depthTexture);

    // Create shader resource view for depth texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // Adjust if your depth format is different (e.g., DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    ID3D11ShaderResourceView* depthSRV = nullptr;
    HRESULT hr = device->CreateShaderResourceView(depthTexture, &srvDesc, &depthSRV);
    if (FAILED(hr))
        return false;
    std::cout << "hellow2!\n";
    // Save original pipeline state
    ID3D11RenderTargetView* originalRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    ID3D11DepthStencilView* originalDSV = nullptr;
    context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, &originalDSV);

    // Set up viewport
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<FLOAT>(MC::windowSize.x);
    viewport.Height = static_cast<FLOAT>(MC::windowSize.y);
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    // Clear and set render target
    FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    context->ClearRenderTargetView(m_renderTargetView, clearColor);
    context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

    // Set up pipeline states
    context->IASetInputLayout(m_inputLayout);
    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);

    // Bind depth texture and sampler
    context->PSSetShaderResources(0, 1, &depthSRV);
    context->PSSetSamplers(0, 1, &m_samplerState);

    // Disable depth testing
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = FALSE;
    ID3D11DepthStencilState* dsState = nullptr;
    hr = device->CreateDepthStencilState(&dsDesc, &dsState);
    if (SUCCEEDED(hr))
        context->OMSetDepthStencilState(dsState, 0);

    // Draw full-screen quad
    context->Draw(4, 0);

    // Copy render texture to staging texture for CPU access
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    m_renderTexture->GetDesc(&stagingDesc);
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ID3D11Texture2D* stagingTexture = nullptr;
    hr = device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
    if (SUCCEEDED(hr))
    {
        context->CopyResource(stagingTexture, m_renderTexture);

        // Map staging texture
        D3D11_MAPPED_SUBRESOURCE mapped;
        hr = context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);
        if (SUCCEEDED(hr))
        {
            // Convert to RGB for stb_image_write (expects 3 or 4 channels)
            UINT width = static_cast<UINT>(MC::windowSize.x);
            UINT height = static_cast<UINT>(MC::windowSize.y);
            std::vector<uint8_t> imageData(width * height * 3);
            uint8_t* src = static_cast<uint8_t*>(mapped.pData);
            for (UINT y = 0; y < height; ++y)
            {
                for (UINT x = 0; x < width; ++x)
                {
                    UINT srcIdx = y * mapped.RowPitch + x * 4;
                    UINT dstIdx = (y * width + x) * 3;
                    imageData[dstIdx + 0] = src[srcIdx + 0]; // R
                    imageData[dstIdx + 1] = src[srcIdx + 1]; // G
                    imageData[dstIdx + 2] = src[srcIdx + 2]; // B
                }
            }
            context->Unmap(stagingTexture, 0);

            // Save to PNG
            static int counter = 0;
            char filename[64];
            sprintf_s(filename, (Utils::getAssetsPath() + "\\depth_map_%d.png").c_str(), counter++);
            stbi_write_png(filename, width, height, 3, imageData.data(), width * 3);
        }
        stagingTexture->Release();
    }

    // Clean up
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);
    if (dsState) dsState->Release();
    depthSRV->Release();

    // Restore original render targets
    context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, originalDSV);
    for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        if (originalRTVs[i]) originalRTVs[i]->Release();
    if (originalDSV) originalDSV->Release();

    return true;
}

void DepthMapGenerator::Cleanup()
{
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_inputLayout) m_inputLayout->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_renderTexture) m_renderTexture->Release();
    if (m_samplerState) m_samplerState->Release();

    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_inputLayout = nullptr;
    m_vertexBuffer = nullptr;
    m_renderTargetView = nullptr;
    m_renderTexture = nullptr;
    m_samplerState = nullptr;
}