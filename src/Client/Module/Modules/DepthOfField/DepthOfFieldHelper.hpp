#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DepthOfFieldInputBuffer
{
    XMFLOAT2 resolution;
    XMFLOAT2 halfpixel;
    float intensity;
    float focusRange;
    float focusDistance;
    float autoFocus;
    float msaaLevel;
    float _padding[3];
};


class DepthOfFieldHelper
{
public:

    static inline ID3D11PixelShader* pHorizontalShader = nullptr;
    static inline ID3D11PixelShader* pVerticalShader = nullptr;
    static inline ID3D11VertexShader* pVertexShader = nullptr;
    static inline ID3D11InputLayout* pInputLayout = nullptr;

    static inline ID3D11SamplerState* pSampler = nullptr;
    static inline ID3D11Buffer* pVertexBuffer = nullptr;
    static inline ID3D11Buffer* pConstantBuffer = nullptr;
    static inline DepthOfFieldInputBuffer constantBuffer;


    // Persistent intermediate textures
    static inline ID3D11Texture2D* pIntermediateTexture1 = nullptr;
    static inline ID3D11Texture2D* pIntermediateTexture2 = nullptr;
    static inline ID3D11RenderTargetView* pIntermediateRTV1 = nullptr;
    static inline ID3D11RenderTargetView* pIntermediateRTV2 = nullptr;
    static inline ID3D11ShaderResourceView* pIntermediateSRV1 = nullptr;
    static inline ID3D11ShaderResourceView* pIntermediateSRV2 = nullptr;
    static inline UINT currentTextureWidth = 0;
    static inline UINT currentTextureHeight = 0;

    // Cached render states
    static inline ID3D11DepthStencilState* pDepthStencilState = nullptr;
    static inline ID3D11BlendState* pBlendState = nullptr;
    static inline ID3D11RasterizerState* pRasterizerState = nullptr;

    static inline ID3D11ShaderResourceView* pDepthMapSRV = nullptr;
    static inline bool isMSAADepth = false;
    static inline int msaaSampleCount = 1;


    // RAII
    static void InitializePipeline();
    static void Cleanup();

    static void RenderToRTV(ID3D11RenderTargetView*, ID3D11ShaderResourceView*, XMFLOAT2);

    static void RenderDepthOfField(ID3D11RenderTargetView*, float, float, float, bool);


private:
    static void SetupShaderResources(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* pColorSRV);
};