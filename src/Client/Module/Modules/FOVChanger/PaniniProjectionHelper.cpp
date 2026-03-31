#include "PaniniProjectionHelper.hpp"
#include <d3dcompiler.h>
#include <cmath>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../../../Utils/Logger/Logger.hpp"

// Panini projection post-process shader
// Based on the Pannini-Shufelt cylindrical stereographic projection.
// Reference: Unity HDRP/URP PaniniProjection.shader, Thomas K. Sharpless (PanoTools)
const char* paniniPixelShaderSrc = R"(
cbuffer PaniniParams : register(b0)
{
    float2 viewExtents;  // (tan(fov/2) * aspect, tan(fov/2))
    float d;             // panini strength [0, 1]
    float cropScale;     // uniform scale to fill screen after projection
};

Texture2D sceneTexture : register(t0);
SamplerState samplerState : register(s0);

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

// General Panini projection for arbitrary d in [0, 1+].
// Maps a view-space 2D position to its Panini-projected position.
// When d=0, this is identity (rectilinear). When d=1, classical Pannini.
float2 PaniniProjection(float2 view_pos, float panini_d)
{
    float view_dist = 1.0 + panini_d;
    float view_dist_sq = view_dist * view_dist;
    float view_hyp_sq = view_pos.x * view_pos.x + view_dist_sq;

    float isect_D = view_pos.x * panini_d;
    float isect_discrim = view_hyp_sq - isect_D * isect_D;

    float cyl_dist_minus_d = (-isect_D * view_pos.x + view_dist * sqrt(isect_discrim)) / view_hyp_sq;
    float cyl_dist = cyl_dist_minus_d + panini_d;

    float2 cyl_pos = view_pos * (cyl_dist / view_dist);
    return cyl_pos / cyl_dist_minus_d;
}

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    // Convert UV [0,1] to centered view-space coordinates
    // cropScale zooms out to prevent black borders at edges
    float2 view_pos = (2.0 * input.Tex - 1.0) * viewExtents * cropScale;

    // Apply Panini projection
    float2 proj_pos = PaniniProjection(view_pos, d);

    // Convert back to normalized UV coordinates
    float2 proj_ndc = proj_pos / viewExtents;
    float2 sample_uv = proj_ndc * 0.5 + 0.5;

    // Black for out-of-bounds samples (shouldn't happen with proper cropScale)
    if (any(sample_uv < 0.0) || any(sample_uv > 1.0))
        return float4(0, 0, 0, 1);

    return sceneTexture.Sample(samplerState, sample_uv);
}
)";

const char* paniniVertexShaderSrc = R"(
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

struct PaniniCBuffer {
    float viewExtX;
    float viewExtY;
    float d;
    float cropScale;
};

bool PaniniProjectionHelper::Initialize()
{
    Logger::debug("[Panini] Initializing...");
    HRESULT hr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    if (!device) {
        Logger::debug("[Panini] Device is nullptr");
        return false;
    }

    if (!CompileShader(paniniVertexShaderSrc, "mainVS", "vs_5_0", &vsBlob))
        return false;

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.put());
    if (FAILED(hr)) {
        vsBlob->Release();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,              D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(float)*3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.put());
    vsBlob->Release();
    if (FAILED(hr))
        return false;

    if (!CompileShader(paniniPixelShaderSrc, "mainPS", "ps_5_0", &psBlob))
        return false;
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.put());
    psBlob->Release();
    if (FAILED(hr))
        return false;

    // Constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(PaniniCBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.put());
    if (FAILED(hr))
        return false;

    // Fullscreen quad vertices (clip-space positions + UVs)
    struct Vertex { float x, y, z, u, v; };
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
    hr = device->CreateBuffer(&vbDesc, &initData, m_vertexBuffer.put());
    if (FAILED(hr))
        return false;

    // Depth/stencil: disabled
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = false;
    dsd.StencilEnable = false;
    hr = device->CreateDepthStencilState(&dsd, m_depthStencilState.put());
    if (FAILED(hr))
        return false;

    // Blend: opaque (no alpha blending — we're replacing the entire frame)
    D3D11_BLEND_DESC bd{};
    bd.AlphaToCoverageEnable = false;
    bd.RenderTarget[0].BlendEnable = false;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&bd, m_blendState.put());
    if (FAILED(hr))
        return false;

    // Rasterizer: no culling, no depth clip
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = false;
    rd.ScissorEnable = false;
    hr = device->CreateRasterizerState(&rd, m_rasterizerState.put());
    if (FAILED(hr))
        return false;

    // Linear clamp sampler for smooth UV remapping
    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&sampDesc, m_samplerState.put());
    if (FAILED(hr))
        return false;

    initialized = true;
    Logger::debug("[Panini] Initialized successfully");
    return true;
}

bool PaniniProjectionHelper::CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(srcData, strlen(srcData), nullptr, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            Logger::debug("[Panini] Shader compile error: {}", (char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    if (errorBlob)
        errorBlob->Release();
    return true;
}

// Compute the crop scale factor so the Panini-projected image fills the screen.
// This is the ratio of Panini-projected edge extent to rectilinear edge extent.
static float CalcCropScale(float fovY_rad, float d, float aspect)
{
    float viewExtY = tanf(fovY_rad * 0.5f);
    float viewExtX = aspect * viewExtY;

    float viewDist = 1.0f + d;
    float projHyp = sqrtf(viewExtX * viewExtX + 1.0f);
    float cylDistMinusD = 1.0f / projHyp;
    float cylDist = cylDistMinusD + d;

    // Crop extents = viewExtents * cylDistMinusD * (viewDist / cylDist)
    // Scale factor = cropExtent / viewExtent = cylDistMinusD * viewDist / cylDist
    return cylDistMinusD * viewDist / cylDist;
}

void PaniniProjectionHelper::Render(ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* sceneSRV,
                                     float fovDegrees, float d, float cropToFit)
{
    ID3D11DeviceContext* context = SwapchainHook::context.get();
    if (!context || !rtv || !sceneSRV)
        return;

    // Get dimensions from the RTV's texture
    ID3D11Resource* resource = nullptr;
    rtv->GetResource(&resource);
    auto* texture = static_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc{};
    texture->GetDesc(&desc);
    resource->Release();

    float aspect = static_cast<float>(desc.Width) / static_cast<float>(desc.Height);
    float fovY_rad = fovDegrees * 3.14159265358979f / 180.0f;
    float viewExtY = tanf(fovY_rad * 0.5f);
    float viewExtX = aspect * viewExtY;

    float tightScale = CalcCropScale(fovY_rad, d, aspect);
    float cropScale = 1.0f + (tightScale - 1.0f) * cropToFit;

    // ── Full D3D11 state save ──
    // Preserve the game's pipeline state so subsequent rendering (D2D, ImGui)
    // isn't affected by our fullscreen quad draw.

    // Output merger
    ID3D11RenderTargetView* origRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ID3D11DepthStencilView* origDSV = nullptr;
    context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, origRTVs, &origDSV);

    ID3D11BlendState* origBlendState = nullptr;
    FLOAT origBlendFactor[4] = {};
    UINT origSampleMask = 0;
    context->OMGetBlendState(&origBlendState, origBlendFactor, &origSampleMask);

    ID3D11DepthStencilState* origDepthStencilState = nullptr;
    UINT origStencilRef = 0;
    context->OMGetDepthStencilState(&origDepthStencilState, &origStencilRef);

    // Rasterizer
    ID3D11RasterizerState* origRasterizerState = nullptr;
    context->RSGetState(&origRasterizerState);

    UINT origNumViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT origViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
    context->RSGetViewports(&origNumViewports, origViewports);

    // Input assembler
    ID3D11InputLayout* origInputLayout = nullptr;
    context->IAGetInputLayout(&origInputLayout);

    D3D11_PRIMITIVE_TOPOLOGY origTopology;
    context->IAGetPrimitiveTopology(&origTopology);

    ID3D11Buffer* origVB = nullptr;
    UINT origVBStride = 0, origVBOffset = 0;
    context->IAGetVertexBuffers(0, 1, &origVB, &origVBStride, &origVBOffset);

    // Shaders
    ID3D11VertexShader* origVS = nullptr;
    context->VSGetShader(&origVS, nullptr, nullptr);

    ID3D11PixelShader* origPS = nullptr;
    context->PSGetShader(&origPS, nullptr, nullptr);

    ID3D11Buffer* origPSCB = nullptr;
    context->PSGetConstantBuffers(0, 1, &origPSCB);

    ID3D11ShaderResourceView* origPSSRV = nullptr;
    context->PSGetShaderResources(0, 1, &origPSSRV);

    ID3D11SamplerState* origPSSampler = nullptr;
    context->PSGetSamplers(0, 1, &origPSSampler);

    // ── Set our pipeline state and draw ──

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(desc.Width);
    vp.Height = static_cast<float>(desc.Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);

    context->OMSetRenderTargets(1, &rtv, nullptr);
    context->OMSetDepthStencilState(m_depthStencilState.get(), 0);
    context->OMSetBlendState(m_blendState.get(), nullptr, 0xffffffff);
    context->RSSetState(m_rasterizerState.get());

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(m_constantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        auto* pData = static_cast<PaniniCBuffer*>(mapped.pData);
        pData->viewExtX = viewExtX;
        pData->viewExtY = viewExtY;
        pData->d = d;
        pData->cropScale = cropScale;
        context->Unmap(m_constantBuffer.get(), 0);
    }

    context->IASetInputLayout(m_inputLayout.get());
    ID3D11Buffer* vb = m_vertexBuffer.get();
    context->IASetVertexBuffers(0, 1, &vb, &VERTEX_STRIDE, &VERTEX_OFFSET);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.get(), nullptr, 0);

    ID3D11Buffer* cbBuf = m_constantBuffer.get();
    context->PSSetConstantBuffers(0, 1, &cbBuf);
    context->PSSetShaderResources(0, 1, &sceneSRV);
    ID3D11SamplerState* sampler = m_samplerState.get();
    context->PSSetSamplers(0, 1, &sampler);

    context->Draw(4, 0);

    // Unbind SRV
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    // ── Full D3D11 state restore ──

    context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, origRTVs, origDSV);
    context->OMSetBlendState(origBlendState, origBlendFactor, origSampleMask);
    context->OMSetDepthStencilState(origDepthStencilState, origStencilRef);
    context->RSSetState(origRasterizerState);
    context->RSSetViewports(origNumViewports, origViewports);
    context->IASetInputLayout(origInputLayout);
    context->IASetPrimitiveTopology(origTopology);
    context->IASetVertexBuffers(0, 1, &origVB, &origVBStride, &origVBOffset);
    context->VSSetShader(origVS, nullptr, 0);
    context->PSSetShader(origPS, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &origPSCB);
    context->PSSetShaderResources(0, 1, &origPSSRV);
    context->PSSetSamplers(0, 1, &origPSSampler);

    // Release saved COM references
    if (origDSV) origDSV->Release();
    for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        if (origRTVs[i]) origRTVs[i]->Release();
    }
    if (origBlendState) origBlendState->Release();
    if (origDepthStencilState) origDepthStencilState->Release();
    if (origRasterizerState) origRasterizerState->Release();
    if (origInputLayout) origInputLayout->Release();
    if (origVB) origVB->Release();
    if (origVS) origVS->Release();
    if (origPS) origPS->Release();
    if (origPSCB) origPSCB->Release();
    if (origPSSRV) origPSSRV->Release();
    if (origPSSampler) origPSSampler->Release();
}

ID3D11ShaderResourceView* PaniniProjectionHelper::CopyBackbufferToSRV(ID3D11Texture2D* backbuffer)
{
    if (!backbuffer) return nullptr;

    ID3D11Device* device = SwapchainHook::d3d11Device.get();
    ID3D11DeviceContext* ctx = SwapchainHook::context.get();
    if (!device || !ctx) return nullptr;

    D3D11_TEXTURE2D_DESC srcDesc{};
    backbuffer->GetDesc(&srcDesc);

    // Recreate intermediate texture if dimensions changed
    if (!m_intermediateTex || srcDesc.Width != m_intermediateWidth || srcDesc.Height != m_intermediateHeight) {
        m_intermediateTex = nullptr;
        m_intermediateSRV = nullptr;

        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = srcDesc.Width;
        texDesc.Height = srcDesc.Height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = srcDesc.Format;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;

        HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_intermediateTex.put());
        if (FAILED(hr)) {
            Logger::debug("[Panini] Failed to create intermediate texture: 0x{:08X}", static_cast<unsigned>(hr));
            return nullptr;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        hr = device->CreateShaderResourceView(m_intermediateTex.get(), &srvDesc, m_intermediateSRV.put());
        if (FAILED(hr)) {
            Logger::debug("[Panini] Failed to create intermediate SRV: 0x{:08X}", static_cast<unsigned>(hr));
            m_intermediateTex = nullptr;
            return nullptr;
        }

        m_intermediateWidth = srcDesc.Width;
        m_intermediateHeight = srcDesc.Height;
    }

    // MSAA sources need ResolveSubresource; non-MSAA can use CopyResource
    if (srcDesc.SampleDesc.Count > 1) {
        ctx->ResolveSubresource(m_intermediateTex.get(), 0, backbuffer, 0, srcDesc.Format);
    } else {
        ctx->CopyResource(m_intermediateTex.get(), backbuffer);
    }
    return m_intermediateSRV.get();
}
