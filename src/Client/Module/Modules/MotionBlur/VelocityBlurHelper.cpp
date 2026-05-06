#include "VelocityBlurHelper.hpp"
#include <d3dcompiler.h>
#include <windows.h>
#include <algorithm>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../../Hook/Hooks/Render/DirectX/DXGI/UnderUIHooks.hpp"
#include "../DepthOfField/DepthOfFieldHelper.hpp"
#include "SDK/SDK.hpp"
#include <glm/glm/gtc/matrix_transform.hpp>

const char* velocityBlurPixelShaderSrc = R"(
cbuffer VelocityData : register(b0)
{
    row_major matrix prevViewProj;
    row_major matrix currViewProj;
    row_major matrix invViewProj;
    float intensity;
    int numSamples;
    float2 padding;
};

Texture2D sceneTexture : register(t0);
Texture2D<float> depthTexture : register(t1);
SamplerState sceneSampler : register(s0);
SamplerState depthSampler : register(s1);

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.Tex;

    float depth = depthTexture.Sample(depthSampler, uv).r;
    if (depth >= 1.0) {
        return sceneTexture.Sample(sceneSampler, uv);
    }

    float4 clipPos = float4(uv * 2.0 - 1.0, depth, 1.0);
    clipPos.y = -clipPos.y;

    float4 worldPos = mul(clipPos, invViewProj);
    worldPos /= worldPos.w;

    float4 prevClipPos = mul(worldPos, prevViewProj);
    prevClipPos /= prevClipPos.w;
    prevClipPos.y = -prevClipPos.y;

    float2 velocity = (uv - prevClipPos.xy * 0.5 - 0.5) * intensity;

    float4 color = float4(0.0, 0.0, 0.0, 0.0);

    [loop]
    for (int i = 0; i < numSamples; ++i)
    {
        float t = (float)i / (float)(numSamples - 1);
        float2 offset = velocity * (t - 0.5);
        float2 sampleUV = uv + offset;

        if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 &&
            sampleUV.y >= 0.0 && sampleUV.y <= 1.0)
        {
            color += sceneTexture.Sample(sceneSampler, sampleUV);
        }
    }

    color /= (float)numSamples;
    return color;
}
)";

const char* velocityBlurVertexShaderSrc = R"(
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

struct VelocityDataBuffer {
    float prevViewProjection[16];
    float currViewProjection[16];
    float invViewProjection[16];
    float intensity;
    int numSamples;
    float padding[2];
};

static bool s_hasPrevMatrix = false;

bool VelocityBlurHelper::Initialize()
{
    Logger::debug("[VelocityBlur] Initializing");
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (!CompileShader(velocityBlurVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;

    ID3D11Device* m_device = SwapchainHook::d3d11Device.get();
    if (!m_device) {
        vsBlob->Release();
        return false;
    }

    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.put());
    if (FAILED(hr)) {
        vsBlob->Release();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float)*3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = m_device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.put());
    vsBlob->Release();
    if (FAILED(hr)) return false;

    if (!CompileShader(velocityBlurPixelShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.put());
    psBlob->Release();
    if (FAILED(hr)) return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(VelocityDataBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.put());
    if (FAILED(hr)) return false;

    struct Vertex { float x, y, z; float u, v; };
    Vertex vertices[] = {
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
    if (FAILED(hr)) return false;

    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    hr = m_device->CreateDepthStencilState(&dsd, m_depthStencilState.put());
    if (FAILED(hr)) return false;

    D3D11_BLEND_DESC bd{};
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = false;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = m_device->CreateBlendState(&bd, m_blendState.put());
    if (FAILED(hr)) return false;

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    hr = m_device->CreateRasterizerState(&rd, m_rasterizerState.put());
    if (FAILED(hr)) return false;

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = m_device->CreateSamplerState(&sampDesc, m_samplerState.put());
    if (FAILED(hr)) return false;

    memset(&m_cachedViewport, 0, sizeof(m_cachedViewport));
    memset(m_prevViewProjection, 0, sizeof(m_prevViewProjection));
    m_prevViewProjection[0] = m_prevViewProjection[5] = m_prevViewProjection[10] = m_prevViewProjection[15] = 1.0f;

    s_hasPrevMatrix = false;
    return true;
}

bool VelocityBlurHelper::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    UINT compileFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(srcData, strlen(srcData), nullptr, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            Logger::debug((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    if (errorBlob) errorBlob->Release();
    return true;
}

void VelocityBlurHelper::Reset()
{
    s_hasPrevMatrix = false;
    memset(m_prevViewProjection, 0, sizeof(m_prevViewProjection));
    m_prevViewProjection[0] = m_prevViewProjection[5] = m_prevViewProjection[10] = m_prevViewProjection[15] = 1.0f;

    if (m_offscreenRTV) { m_offscreenRTV.reset(); }
    if (m_offscreenSRV) { m_offscreenSRV.reset(); }
    if (m_offscreenTexture) { m_offscreenTexture.reset(); }

    Logger::debug("[VelocityBlur] State reset");
}

void VelocityBlurHelper::Render(ID3D11RenderTargetView* rtv, winrt::com_ptr<ID3D11ShaderResourceView>& frame)
{
    ID3D11DeviceContext* context = SwapchainHook::context.get();
    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!context || !device || !rtv || !frame) return;

    if (!DepthOfFieldHelper::pDepthMapSRV || DepthOfFieldHelper::isMSAADepth) return;

    ID3D11Resource* resource = nullptr;
    rtv->GetResource(&resource);
    ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);
    resource->Release();

    ID3D11RenderTargetView* originalRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    UINT numRTVs = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    ID3D11DepthStencilView* originalDSV = nullptr;
    context->OMGetRenderTargets(numRTVs, originalRTVs, &originalDSV);

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

    glm::mat4 currVP;
    if (SDK::clientInstance) {
        auto minecraftGame = SDK::clientInstance->getMinecraftGame();
        if (minecraftGame) {
            auto gameRenderer = minecraftGame->getGameRenderer();
            if (gameRenderer) {
                const glm::mat4& viewMatrix = gameRenderer->getLastViewMatrix();
                const glm::mat4& projMatrix = gameRenderer->getLastProjectionMatrix();
                currVP = projMatrix * viewMatrix;
            } else {
                currVP = Matrix::getMatrixCorrection(MC::Transform.modelView);
            }
        } else {
            currVP = Matrix::getMatrixCorrection(MC::Transform.modelView);
        }
    } else {
        currVP = Matrix::getMatrixCorrection(MC::Transform.modelView);
    }

    glm::mat4 currVPTransposed = glm::transpose(currVP);
    glm::mat4 invCurrVP = glm::inverse(currVP);
    glm::mat4 invCurrVPTransposed = glm::transpose(invCurrVP);

    if (!s_hasPrevMatrix) {
        memcpy(m_prevViewProjection, &currVPTransposed[0][0], sizeof(m_prevViewProjection));
        s_hasPrevMatrix = true;

        if (originalDSV) originalDSV->Release();
        for (UINT i = 0; i < numRTVs; ++i) {
            if (originalRTVs[i]) originalRTVs[i]->Release();
        }
        return;
    }

    auto module = ModuleManager::getModule("Motion Blur");
    float intensity = module->getOps<float>("intensity_velocity");
    int numSamples = static_cast<int>(module->getOps<float>("samples"));
    numSamples = std::clamp(numSamples, 8, 128);

    context->RSSetViewports(1, &m_cachedViewport);

    context->OMSetRenderTargets(1, &rtv, originalDSV);

    context->OMSetDepthStencilState(m_depthStencilState.get(), 0);
    context->OMSetBlendState(m_blendState.get(), nullptr, 0xffffffff);
    context->RSSetState(m_rasterizerState.get());

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_constantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        VelocityDataBuffer* pData = (VelocityDataBuffer*)mappedResource.pData;
        pData->intensity = intensity;
        pData->numSamples = numSamples;
        memcpy(pData->prevViewProjection, m_prevViewProjection, sizeof(pData->prevViewProjection));
        memcpy(pData->currViewProjection, &currVPTransposed[0][0], sizeof(pData->currViewProjection));
        memcpy(pData->invViewProjection, &invCurrVPTransposed[0][0], sizeof(pData->invViewProjection));
        context->Unmap(m_constantBuffer.get(), 0);
    }

    context->IASetInputLayout(m_inputLayout.get());
    ID3D11Buffer* vertexBuffer = m_vertexBuffer.get();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &VERTEX_STRIDE, &VERTEX_OFFSET);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    ID3D11Buffer* constantBuffer = m_constantBuffer.get();
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    ID3D11ShaderResourceView* sceneSRV = frame.get();
    context->PSSetShaderResources(0, 1, &sceneSRV);

    ID3D11ShaderResourceView* depthSRV = DepthOfFieldHelper::pDepthMapSRV;
    context->PSSetShaderResources(1, 1, &depthSRV);

    ID3D11SamplerState* samplerPtr = m_samplerState.get();
    context->PSSetSamplers(0, 1, &samplerPtr);
    context->PSSetSamplers(1, 1, &samplerPtr);

    context->Draw(4, 0);

    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, nullSRV);

    if (originalDSV) originalDSV->Release();
    for (UINT i = 0; i < numRTVs; ++i) {
        if (originalRTVs[i]) originalRTVs[i]->Release();
    }

    memcpy(m_prevViewProjection, &currVPTransposed[0][0], sizeof(m_prevViewProjection));
}
