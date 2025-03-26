#include "RealMotionBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <assert.h>
#include "Hook/Hooks/Render/SwapchainHook.hpp"

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

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.Tex;
    float4 color = sceneTexture.Sample(samplerState, uv);

    float4 H = float4(uv.x * 2 - 1, (1 - uv.y) * 2 - 1, 0, 1);

    float4 worldPos = mul(H, invWorldViewProjection);
    worldPos /= worldPos.w;

    float4 previousPos = mul(worldPos, preWorldViewProjection);
    previousPos /= previousPos.w;

    float2 velocity = (H.xy - previousPos.xy) / 2.0;
    velocity.y *= -1;

    velocity *= intensity;

    float4 finalColor = color;
    int numSamples = 6;
    [unroll]
    for (int i = 1; i <= numSamples; i++) {
        float2 sampleUV = uv + velocity * (i / (float)numSamples);
        finalColor += sceneTexture.Sample(samplerState, sampleUV);
    }
    return finalColor / (numSamples + 1);
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
    ID3D11Device* m_device = SwapchainHook::d3d11Device;
    if (!m_device) {
        Logger::debug("Device is nullptr");
        return false;
    }
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

    if (!CompileShader(realMotionBlurPixelShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);
    psBlob->Release();
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(CameraDataBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
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
    hr = m_device->CreateBuffer(&vbDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr))
        return false;

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
    ID3D11DeviceContext* context = SwapchainHook::context;
    ID3D11Device* device = SwapchainHook::d3d11Device;
    if (!context || !device || !rtv) {
        return;
    }

    ID3D11Resource* resource = nullptr;
    rtv->GetResource(&resource);
    ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    resource->Release();

    D3D11_VIEWPORT viewport = {0};
    viewport.Width = static_cast<float>(desc.Width);
    viewport.Height = static_cast<float>(desc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    context->OMSetRenderTargets(1, &rtv, nullptr);

    // Use the single frame provided
    ID3D11ShaderResourceView* sceneSRV = frame.get();

    glm::mat4 currWVP = Matrix::getMatrixCorrection(MC::Transform.modelView);
    glm::mat4 invCurrWVP = glm::inverse(currWVP);

    auto module = ModuleManager::getModule("Motion Blur");
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        CameraDataBuffer* pData = (CameraDataBuffer*)mappedResource.pData;
        pData->intensity = module->settings.getSettingByName<float>("intensity2")->value * (1.0f / FlarialGUI::frameFactor);
        memcpy(pData->preWorldViewProjection, m_prevWorldMatrix, sizeof(pData->preWorldViewProjection));
        memcpy(pData->invWorldViewProjection, &invCurrWVP[0][0], sizeof(pData->invWorldViewProjection));
        context->Unmap(m_constantBuffer, 0);
    }

    context->IASetInputLayout(m_inputLayout);
    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_constantBuffer);

    context->PSSetShaderResources(0, 1, &sceneSRV);
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

    context->Draw(4, 0);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    context->PSSetShaderResources(0, 1, nullSRV);
    if (sampler)
        sampler->Release();

    memcpy(m_prevWorldMatrix, &currWVP[0][0], sizeof(m_prevWorldMatrix));
}
