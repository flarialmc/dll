#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DepthOfFieldInputBuffer
{
    XMFLOAT2 resolution;
    XMFLOAT2 offset;
    XMFLOAT2 halfpixel;
    float intensity;
    float focusRange;
    float focusDistance;
    float autoFocus;
    float _padding;
    float _padding2;
};


class DepthOfFieldHelper
{
public:

    static inline ID3D11PixelShader* pGaussianBlurHorizontalShader = nullptr;
    static inline ID3D11PixelShader* pGaussianBlurVerticalShader = nullptr;
    static inline ID3D11PixelShader* pDepthBlurHorizontalShader = nullptr;
    static inline ID3D11PixelShader* pDepthBlurVerticalShader = nullptr;
    static inline ID3D11VertexShader* pVertexShader = nullptr;
    static inline ID3D11InputLayout* pInputLayout = nullptr;
    static inline ID3D11ComputeShader* pDepthResolveComputeShader = nullptr;

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

    // Keep the depth map SRV for depth of field calculations
    static inline ID3D11ShaderResourceView* pDepthMapSRV = nullptr;

    // Depth blur intermediate textures
    static inline ID3D11Texture2D* pDepthBlurTexture1 = nullptr;
    static inline ID3D11Texture2D* pDepthBlurTexture2 = nullptr;
    static inline ID3D11RenderTargetView* pDepthBlurRTV1 = nullptr;
    static inline ID3D11RenderTargetView* pDepthBlurRTV2 = nullptr;
    static inline ID3D11ShaderResourceView* pDepthBlurSRV1 = nullptr;
    static inline ID3D11ShaderResourceView* pDepthBlurSRV2 = nullptr;


    // RAII
    static void InitializePipeline();
    static void Cleanup();

    static void RenderToRTV(ID3D11RenderTargetView*, ID3D11ShaderResourceView*, XMFLOAT2);

    static void RenderDepthOfField(ID3D11RenderTargetView*, int, float, float, float, bool, float);

    // Function to resolve MSAA depth buffer using compute shader
    static bool ResolveDepthWithComputeShader(ID3D11DeviceContext* pContext, ID3D11Texture2D* pMSAADepthTexture, ID3D11Texture2D* pResolvedDepthTexture);

private:
    static bool EnsureIntermediateTextures(UINT width, UINT height);
    static void ReleaseIntermediateTextures();
};