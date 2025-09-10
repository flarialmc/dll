#include "RealMotionBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <unknwn.h>
#include <assert.h>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../../Hook/Hooks/Render/DirectX/DXGI/UnderUIHooks.hpp"

const char* realMotionBlurPixelShaderSrc = R"(
cbuffer CameraData : register(b0)
{
    row_major matrix preWorldViewProjection;
    row_major matrix invWorldViewProjection;
    float intensity;
    float3 padding;
};

Texture2D sceneTexture : register(t0);
SamplerState samplerState : register(s0);
Texture2D depthTexture : register(t1);
SamplerState depthSampler : register(s1);

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float2 sampleCoord = input.Tex;

    float depth = 1.0;

    float4 H = float4(sampleCoord.x * 2 - 1,
                      (1 - sampleCoord.y) * 2 - 1,
                      depth * 2 - 1,
                      1);

    float4 worldPos = mul(H, invWorldViewProjection);
    worldPos /= worldPos.w;

    float4 previousPos = mul(worldPos, preWorldViewProjection);
    previousPos /= previousPos.w;

    int numSamples = 32;
    float2 velocity = ((H.xy - previousPos.xy) / numSamples) * float2(0.5f, -0.5f);
    velocity *= intensity;

    float4 color = sceneTexture.Sample(samplerState, sampleCoord);
    [unroll]
    for (int i = 1; i < numSamples; ++i)
    {
        sampleCoord += velocity;
        sampleCoord = clamp(sampleCoord, 0.0, 1.0);
        color += sceneTexture.Sample(samplerState, sampleCoord);
    }

    return color / (numSamples);
}


)";

const char* realDrawTextureVertexShaderSrc = R"(
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

struct CameraDataBuffer {
    float preWorldViewProjection[16];
    float invWorldViewProjection[16];
    float intensity;
    float padding[3];
};

bool RealMotionBlurHelper::Initialize()
{
    Logger::debug("Initializing motion blur helper with WVP matrices...");
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (!CompileShader(realDrawTextureVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;
    ID3D11Device* m_device = SwapchainHook::d3d11Device.get();
    if (!m_device) {
        Logger::debug("Device is nullptr");
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

    if (!CompileShader(realMotionBlurPixelShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.put());
    psBlob->Release();
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(CameraDataBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.put());
    if (FAILED(hr))
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to create constant buffer: 0x%08X\n", hr);
        OutputDebugStringA(errorMsg);
        return false;
    }

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
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    hr = m_device->CreateSamplerState(&sampDesc, m_samplerState.put());
    if (FAILED(hr))
        return false;

    // Initialize cached viewport to zero to ensure first update
    memset(&m_cachedViewport, 0, sizeof(m_cachedViewport));

    memset(&m_prevWorldMatrix, 0, sizeof(m_prevWorldMatrix));
    m_prevWorldMatrix[0] = m_prevWorldMatrix[5] = m_prevWorldMatrix[10] = m_prevWorldMatrix[15] = 1.0f;

    return true;
}

bool RealMotionBlurHelper::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
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
    if (errorBlob)
        errorBlob->Release();
    return true;
}

void RealMotionBlurHelper::Render(ID3D11RenderTargetView* rtv, winrt::com_ptr<ID3D11ShaderResourceView>& frame)
{
    ID3D11DeviceContext* context = SwapchainHook::context.get();
    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!frame)
    {
        Logger::debug("Frame is null");
        return;
    }
    if (!context || !device || !rtv)
    {
        return;
    }

    // Retrieve the texture description from the RTV's resource.
    ID3D11Resource* resource = nullptr;
    rtv->GetResource(&resource);
    ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);
    resource->Release();

    ID3D11RenderTargetView* originalRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    UINT numRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    ID3D11DepthStencilView* originalDepthStencilView = nullptr;
    context->OMGetRenderTargets(numRenderTargets, originalRenderTargetViews, &originalDepthStencilView);


    // Only update viewport if RTV dimensions changed
    float currentWidth = static_cast<float>(desc.Width);
    float currentHeight = static_cast<float>(desc.Height);
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

    // Use cached render states (no recreation overhead)
    context->OMSetDepthStencilState(m_depthStencilState.get(), 0);
    context->OMSetBlendState(m_blendState.get(), nullptr, 0xffffffff);
    context->RSSetState(m_rasterizerState.get());

    // -------------------------------
    // Process scene and update constant buffer.
    // -------------------------------
    ID3D11ShaderResourceView* sceneSRV = frame.get();

    ID3D11Resource* frameResource = nullptr;
    frame->GetResource(&frameResource);
    ID3D11Texture2D* frameTexture = static_cast<ID3D11Texture2D*>(frameResource);
    D3D11_TEXTURE2D_DESC frameDesc;
    frameTexture->GetDesc(&frameDesc);
    frameResource->Release();

    glm::mat4 currWVP = Matrix::getMatrixCorrection(MC::Transform.modelView);
    glm::mat4 invCurrWVP = glm::inverse(currWVP);

    auto module = ModuleManager::getModule("Motion Blur");
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        CameraDataBuffer* pData = (CameraDataBuffer*)mappedResource.pData;
        pData->intensity = module->getOps<float>("intensity");
        memcpy(pData->preWorldViewProjection, m_prevWorldMatrix, sizeof(pData->preWorldViewProjection));
        memcpy(pData->invWorldViewProjection, &invCurrWVP[0][0], sizeof(pData->invWorldViewProjection));
        context->Unmap(m_constantBuffer.get(), 0);
    }

    // -------------------------------
    // Set up pipeline states.
    // -------------------------------
    context->IASetInputLayout(m_inputLayout.get());
    ID3D11Buffer* vertexBuffer = m_vertexBuffer.get();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &VERTEX_STRIDE, &VERTEX_OFFSET);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    ID3D11Buffer* constantBuffer = m_constantBuffer.get();
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    context->PSSetShaderResources(0, 1, &sceneSRV);

    // Use cached sampler state
    ID3D11SamplerState* samplerPtr = m_samplerState.get();
    context->PSSetSamplers(0, 1, &samplerPtr);
    // Also set the same sampler for the depth SRV on slot 1.
    context->PSSetSamplers(1, 1, &samplerPtr);

    // -------------------------------
    // Draw the geometry.
    // -------------------------------
    context->Draw(4, 0);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    context->PSSetShaderResources(0, 1, nullSRV);
    context->PSSetShaderResources(1, 1, nullSRV);

    // Cached states are cleaned up in ResizeHook::cleanShit
    if (originalDepthStencilView) originalDepthStencilView->Release();
    for (UINT i = 0; i < numRenderTargets; ++i) {
        if (originalRenderTargetViews[i]) originalRenderTargetViews[i]->Release();
    }
    memcpy(m_prevWorldMatrix, &currWVP[0][0], sizeof(m_prevWorldMatrix));
}
