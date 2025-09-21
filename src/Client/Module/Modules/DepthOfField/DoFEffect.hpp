#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cassert>
#include "Modules/MotionBlur/MotionBlur.hpp"

using namespace DirectX;
using namespace Microsoft::WRL;

class DepthOfFieldShader {
public:
    static inline ID3D11Device* d3d11Device = nullptr;
    static inline ID3D11DeviceContext* d3d11Context = nullptr;

    ID3D11VertexShader* pVertexShader;
    ID3D11PixelShader* pHorizontalBlurShader;
    ID3D11PixelShader* pVerticalBlurShader;
    ID3D11InputLayout* pInputLayout;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pConstantBuffer;
    ID3D11SamplerState* pSamplerState;
    ID3D11DepthStencilState* pDepthStencilState;
    ID3D11BlendState* pBlendState;
    ID3D11RasterizerState* pRasterizerState;

    ID3D11Texture2D* pIntermediateTexture1;
    ID3D11Texture2D* pIntermediateTexture2;
    ID3D11RenderTargetView* pIntermediateRTV1;
    ID3D11RenderTargetView* pIntermediateRTV2;
    ID3D11ShaderResourceView* pIntermediateSRV1;
    ID3D11ShaderResourceView* pIntermediateSRV2;

    static inline ID3D11ShaderResourceView* pDepthMapSRV = nullptr;

    UINT currentTextureWidth;
    UINT currentTextureHeight;

    struct ConstantBuffer {
        XMFLOAT2 resolution;
        XMFLOAT2 halfPixel;
        float baseIntensity;
        float focalDepth;
        float focusRange;
        float maxBlur;
    } constantBuffer;

    static inline const XMFLOAT4 quadVertices[6] = {
        XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f),
        XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
        XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f),
        XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f)
    };

    ID3DBlob* CompileShader(const char* pSrcData, const char* pTarget, const char* shaderName) {
        ID3DBlob* shaderBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompile(pSrcData, strlen(pSrcData), shaderName, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

        if (FAILED(hr)) {
            if (errorBlob) {
                char errorMsg[1024];
                snprintf(errorMsg, sizeof(errorMsg), "Failed to compile %s shader: %s\n", shaderName, (char*)errorBlob->GetBufferPointer());
                OutputDebugStringA(errorMsg);
                errorBlob->Release();
            } else {
                OutputDebugStringA("Unknown shader compilation error!\n");
            }
            return nullptr; // Return null instead of throwing for better control
        }
        return shaderBlob;
    }

public:
    DepthOfFieldShader() : pVertexShader(nullptr), pHorizontalBlurShader(nullptr), pVerticalBlurShader(nullptr),
                          pInputLayout(nullptr), pVertexBuffer(nullptr), pConstantBuffer(nullptr),
                          pSamplerState(nullptr), pDepthStencilState(nullptr), pBlendState(nullptr),
                          pRasterizerState(nullptr), pIntermediateTexture1(nullptr), pIntermediateTexture2(nullptr),
                          pIntermediateRTV1(nullptr), pIntermediateRTV2(nullptr), pIntermediateSRV1(nullptr),
                          pIntermediateSRV2(nullptr), currentTextureWidth(0), currentTextureHeight(0) {
        ZeroMemory(&constantBuffer, sizeof(ConstantBuffer));
    }

    ~DepthOfFieldShader() { Cleanup(); }

    bool Initialize() {
        if (!d3d11Device || !d3d11Context) {
            OutputDebugStringA("DepthOfFieldShader: d3d11Device or d3d11Context is null!\n");
            return false;
        }

        HRESULT hr;

        D3D11_BUFFER_DESC vbDesc = { sizeof(quadVertices), D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
        D3D11_SUBRESOURCE_DATA vbData = { quadVertices, 0, 0 };
        hr = d3d11Device->CreateBuffer(&vbDesc, &vbData, &pVertexBuffer);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create vertex buffer!\n");
            return false;
        }

        D3D11_BUFFER_DESC cbDesc = { sizeof(ConstantBuffer), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
        hr = d3d11Device->CreateBuffer(&cbDesc, nullptr, &pConstantBuffer);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create constant buffer!\n");
            return false;
        }

        ID3DBlob* vsBlob = CompileShader(vertexShaderSrc, "vs_4_0", "VertexShader");
        if (!vsBlob) return false;
        hr = d3d11Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVertexShader);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create vertex shader!\n");
            vsBlob->Release();
            return false;
        }

        ID3DBlob* psHorizontalBlob = CompileShader(gaussianBlurHorizontalShaderSrc, "ps_4_0", "HorizontalBlurShader");
        if (!psHorizontalBlob) return false;
        hr = d3d11Device->CreatePixelShader(psHorizontalBlob->GetBufferPointer(), psHorizontalBlob->GetBufferSize(), nullptr, &pHorizontalBlurShader);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create horizontal blur shader!\n");
            psHorizontalBlob->Release();
            return false;
        }

        ID3DBlob* psVerticalBlob = CompileShader(gaussianBlurVerticalShaderSrc, "ps_4_0", "VerticalBlurShader");
        if (!psVerticalBlob) return false;
        hr = d3d11Device->CreatePixelShader(psVerticalBlob->GetBufferPointer(), psVerticalBlob->GetBufferSize(), nullptr, &pVerticalBlurShader);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create vertical blur shader!\n");
            psVerticalBlob->Release();
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC ied[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} };
        hr = d3d11Device->CreateInputLayout(ied, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pInputLayout);
        vsBlob->Release();
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create input layout!\n");
            return false;
        }

        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        hr = d3d11Device->CreateSamplerState(&samplerDesc, &pSamplerState);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create sampler state!\n");
            return false;
        }

        D3D11_DEPTH_STENCIL_DESC dsDesc = { false, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_ALWAYS, false };
        hr = d3d11Device->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create depth stencil state!\n");
            return false;
        }

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = d3d11Device->CreateBlendState(&blendDesc, &pBlendState);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create blend state!\n");
            return false;
        }

        D3D11_RASTERIZER_DESC rasterDesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE, false, 0, 0.0f, 0.0f, false, false, false, false };
        hr = d3d11Device->CreateRasterizerState(&rasterDesc, &pRasterizerState);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to create rasterizer state!\n");
            return false;
        }

        return true;
    }

    void Render(ID3D11RenderTargetView* pDstRenderTargetView, int iterations, float baseIntensity, float focusRange = 0.1f, float maxBlur = 3.0f) {
    // Comprehensive safety checks
    if (baseIntensity <= 0 || !pDstRenderTargetView || !pDepthMapSRV || !d3d11Context || !pVertexShader ||
        !pHorizontalBlurShader || !pVerticalBlurShader || !pInputLayout || !pVertexBuffer ||
        !pConstantBuffer || !pSamplerState || !pDepthStencilState || !pBlendState || !pRasterizerState) {
        return;
    }

    // Limit iterations for performance
    iterations = std::max(1, std::min(iterations, 2));

    winrt::com_ptr<ID3D11ShaderResourceView> pColorSRV = MotionBlur::BackbufferToSRVExtraMode();
    if (!pColorSRV) {
        OutputDebugStringA("Failed to get backbuffer SRV!\n");
        return;
    }

    ID3D11Texture2D* pBackbuffer;
    pDstRenderTargetView->GetResource((ID3D11Resource**)&pBackbuffer);

    D3D11_TEXTURE2D_DESC desc;
    pBackbuffer->GetDesc(&desc);
    pBackbuffer->Release();

    if (!EnsureIntermediateTextures(desc.Width, desc.Height)) {
        OutputDebugStringA("Failed to ensure intermediate textures!\n");
        return;
    }

    XMFLOAT2 renderSize = XMFLOAT2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
    constantBuffer.resolution = renderSize;
    constantBuffer.halfPixel = XMFLOAT2(0.5f / renderSize.x, 0.5f / renderSize.y);
    constantBuffer.baseIntensity = baseIntensity;
    constantBuffer.focalDepth = 0.5f; // Not used anymore, but keep for shader compatibility
    constantBuffer.focusRange = focusRange;
    constantBuffer.maxBlur = maxBlur;

    // Additional validation before UpdateSubresource
    if (!pConstantBuffer) {
        OutputDebugStringA("pConstantBuffer is null in Render!\n");
        return;
    }
    d3d11Context->UpdateSubresource(pConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);

    ID3D11ShaderResourceView* currentSRV = pColorSRV.get();
    if (!currentSRV) {
        OutputDebugStringA("Current SRV is null!\n");
        return;
    }
    ID3D11RenderTargetView* pIntermediateRTV = pIntermediateRTV1;
    ID3D11ShaderResourceView* pIntermediateSRV = pIntermediateSRV1;
    if (!pIntermediateRTV || !pIntermediateSRV) {
        OutputDebugStringA("Intermediate RTV or SRV is null!\n");
        return;
    }

    d3d11Context->OMSetDepthStencilState(pDepthStencilState, 0);
    d3d11Context->OMSetBlendState(pBlendState, nullptr, 0xffffffff);
    d3d11Context->RSSetState(pRasterizerState);

    d3d11Context->IASetInputLayout(pInputLayout);
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;
    d3d11Context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3d11Context->VSSetShader(pVertexShader, nullptr, 0);
    d3d11Context->PSSetSamplers(0, 1, &pSamplerState);
    d3d11Context->PSSetConstantBuffers(0, 1, &pConstantBuffer);

    ID3D11ShaderResourceView* depthSRVArray[] = { pDepthMapSRV };
    d3d11Context->PSSetShaderResources(1, 1, depthSRVArray);

    int actualIterations = std::max(1, iterations);
    for (int i = 0; i < actualIterations; i++) {
        ID3D11ShaderResourceView* nullSRV = nullptr;
        d3d11Context->PSSetShaderResources(0, 1, &nullSRV);

        d3d11Context->PSSetShader(pHorizontalBlurShader, nullptr, 0);
        ID3D11ShaderResourceView* colorSRVArray[] = { currentSRV };
        d3d11Context->PSSetShaderResources(0, 1, colorSRVArray);
        RenderToRTV(pIntermediateRTV, renderSize, true); // Clear intermediate target

        d3d11Context->PSSetShaderResources(0, 1, &nullSRV);

        d3d11Context->PSSetShader(pVerticalBlurShader, nullptr, 0);
        ID3D11ShaderResourceView* intermediateSRVArray[] = { pIntermediateSRV };
        d3d11Context->PSSetShaderResources(0, 1, intermediateSRVArray);
        if (i == actualIterations - 1) {
            RenderToRTV(pDstRenderTargetView, renderSize, false); // Don't clear final target
        } else {
            if (!pIntermediateRTV2 || !pIntermediateSRV2) {
                OutputDebugStringA("Intermediate RTV2 or SRV2 is null!\n");
                return;
            }
            RenderToRTV(pIntermediateRTV2, renderSize, true); // Clear intermediate target
            std::swap(pIntermediateRTV, pIntermediateRTV2);
            std::swap(pIntermediateSRV, pIntermediateSRV2);
            currentSRV = pIntermediateSRV;
        }
    }

    ID3D11ShaderResourceView* nullSRV = nullptr;
    d3d11Context->PSSetShaderResources(0, 1, &nullSRV);
    d3d11Context->PSSetShaderResources(1, 1, &nullSRV);
}
    
    void RenderToRTV(ID3D11RenderTargetView* pRTV, XMFLOAT2 renderSize, bool clearTarget = true) {
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, renderSize.x, renderSize.y, 0.0f, 1.0f };
        d3d11Context->RSSetViewports(1, &viewport);

        if (clearTarget) {
            FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            d3d11Context->ClearRenderTargetView(pRTV, clearColor);
        }

        d3d11Context->OMSetRenderTargets(1, &pRTV, nullptr);
        d3d11Context->Draw(6, 0);
    }

    bool EnsureIntermediateTextures(UINT width, UINT height) {
        if (currentTextureWidth == width && currentTextureHeight == height &&
            pIntermediateTexture1 && pIntermediateTexture2) {
            return true;
        }

        ReleaseIntermediateTextures();

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr1 = d3d11Device->CreateTexture2D(&desc, nullptr, &pIntermediateTexture1);
        HRESULT hr2 = d3d11Device->CreateTexture2D(&desc, nullptr, &pIntermediateTexture2);
        if (FAILED(hr1) || FAILED(hr2)) {
            OutputDebugStringA("Failed to create intermediate textures!\n");
            return false;
        }

        hr1 = d3d11Device->CreateRenderTargetView(pIntermediateTexture1, nullptr, &pIntermediateRTV1);
        hr2 = d3d11Device->CreateRenderTargetView(pIntermediateTexture2, nullptr, &pIntermediateRTV2);
        if (FAILED(hr1) || FAILED(hr2)) {
            OutputDebugStringA("Failed to create intermediate RTVs!\n");
            return false;
        }

        hr1 = d3d11Device->CreateShaderResourceView(pIntermediateTexture1, nullptr, &pIntermediateSRV1);
        hr2 = d3d11Device->CreateShaderResourceView(pIntermediateTexture2, nullptr, &pIntermediateSRV2);
        if (FAILED(hr1) || FAILED(hr2)) {
            OutputDebugStringA("Failed to create intermediate SRVs!\n");
            return false;
        }

        currentTextureWidth = width;
        currentTextureHeight = height;
        return true;
    }

    void ReleaseIntermediateTextures() {
        if (pIntermediateSRV1) pIntermediateSRV1->Release();
        if (pIntermediateSRV2) pIntermediateSRV2->Release();
        if (pIntermediateRTV1) pIntermediateRTV1->Release();
        if (pIntermediateRTV2) pIntermediateRTV2->Release();
        if (pIntermediateTexture1) pIntermediateTexture1->Release();
        if (pIntermediateTexture2) pIntermediateTexture2->Release();
        pIntermediateSRV1 = pIntermediateSRV2 = nullptr;
        pIntermediateRTV1 = pIntermediateRTV2 = nullptr;
        pIntermediateTexture1 = pIntermediateTexture2 = nullptr;
        currentTextureWidth = currentTextureHeight = 0;
    }

    void Cleanup() {
        ReleaseIntermediateTextures();
        if (pDepthMapSRV) pDepthMapSRV->Release();
        if (pVertexShader) pVertexShader->Release();
        if (pHorizontalBlurShader) pHorizontalBlurShader->Release();
        if (pVerticalBlurShader) pVerticalBlurShader->Release();
        if (pInputLayout) pInputLayout->Release();
        if (pVertexBuffer) pVertexBuffer->Release();
        if (pConstantBuffer) pConstantBuffer->Release();
        if (pSamplerState) pSamplerState->Release();
        if (pDepthStencilState) pDepthStencilState->Release();
        if (pBlendState) pBlendState->Release();
        if (pRasterizerState) pRasterizerState->Release();
        pDepthMapSRV = nullptr;
    }

    static inline const char* vertexShaderSrc = R"(
        struct VS_INPUT {
            float4 pos : POSITION;
        };
        float4 main(VS_INPUT input) : SV_POSITION {
            return input.pos;
        }
        )";

    static inline const char* gaussianBlurHorizontalShaderSrc = R"(
        cbuffer BlurInputBuffer : register(b0)
        {
            float2 resolution;
            float2 halfPixel;
            float baseIntensity;
            float focalDepth;
            float focusRange;
            float maxBlur;
        };

        sampler sampler0 : register(s0);
        Texture2D texture0 : register(t0); // Color texture
        Texture2D texture1 : register(t1); // Depth texture

        float4 main(float4 screenSpace : SV_Position) : SV_TARGET
        {
            float2 uv = screenSpace.xy / resolution;

            // Sample depth at center of screen (focus point)
            float2 centerUV = float2(0.5, 0.5);
            float centerDepth = texture1.Sample(sampler0, centerUV).r;

            // Sample depth at current pixel
            float currentDepth = texture1.Sample(sampler0, uv).r;

            // Calculate blur based on depth difference from center focus point
            float depthDiff = abs(currentDepth - centerDepth);
            float blurRadius = min(depthDiff / focusRange, 1.0) * maxBlur;

            // Early exit for pixels in focus (near center depth)
            if (blurRadius < 0.1) {
                return texture0.Sample(sampler0, uv);
            }

            // Fixed sample count for consistent performance
            float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
            float weightSum = 0.0;

            // Use fewer samples for better performance
            int samples = 5;
            for (int i = -samples; i <= samples; i++) {
                float x = (float)i;
                float weight = 1.0 - abs(x) / (float)(samples + 1);
                float2 sampleOffset = float2(x * halfPixel.x * blurRadius * baseIntensity, 0.0);

                colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
                weightSum += weight;
            }

            return colorSum / weightSum;
        }
        )";
    static inline const char* gaussianBlurVerticalShaderSrc = R"(
        cbuffer BlurInputBuffer : register(b0)
        {
            float2 resolution;
            float2 halfPixel;
            float baseIntensity;
            float focalDepth;
            float focusRange;
            float maxBlur;
        };

        sampler sampler0 : register(s0);
        Texture2D texture0 : register(t0); // Color texture
        Texture2D texture1 : register(t1); // Depth texture

        float4 main(float4 screenSpace : SV_Position) : SV_TARGET
        {
            float2 uv = screenSpace.xy / resolution;

            // Sample depth at center of screen (focus point)
            float2 centerUV = float2(0.5, 0.5);
            float centerDepth = texture1.Sample(sampler0, centerUV).r;

            // Sample depth at current pixel
            float currentDepth = texture1.Sample(sampler0, uv).r;

            // Calculate blur based on depth difference from center focus point
            float depthDiff = abs(currentDepth - centerDepth);
            float blurRadius = min(depthDiff / focusRange, 1.0) * maxBlur;

            // Early exit for pixels in focus (near center depth)
            if (blurRadius < 0.1) {
                return texture0.Sample(sampler0, uv);
            }

            // Fixed sample count for consistent performance
            float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
            float weightSum = 0.0;

            // Use fewer samples for better performance
            int samples = 5;
            for (int i = -samples; i <= samples; i++) {
                float y = (float)i;
                float weight = 1.0 - abs(y) / (float)(samples + 1);
                float2 sampleOffset = float2(0.0, y * halfPixel.y * blurRadius * baseIntensity);

                colorSum += texture0.Sample(sampler0, uv + sampleOffset) * weight;
                weightSum += weight;
            }

            return colorSum / weightSum;
        }
        )";
};