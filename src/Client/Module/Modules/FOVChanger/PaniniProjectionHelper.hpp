#pragma once
#include <d3d11.h>
#include <winrt/base.h>

class PaniniProjectionHelper
{
public:
    static bool Initialize();
    static bool CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);
    static void Render(ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* sceneSRV, float fovDegrees, float d, float cropToFit);

    // Copies the current backbuffer to an intermediate texture and returns its SRV.
    // Required on DX12 where SavedD3D11BackBuffer IS the backbuffer (not a copy).
    // Also works on DX11 as a universal fallback.
    static ID3D11ShaderResourceView* CopyBackbufferToSRV(ID3D11Texture2D* backbuffer);

    static inline bool initialized = false;

    static inline winrt::com_ptr<ID3D11PixelShader>       m_pixelShader;
    static inline winrt::com_ptr<ID3D11VertexShader>      m_vertexShader;
    static inline winrt::com_ptr<ID3D11InputLayout>       m_inputLayout;
    static inline winrt::com_ptr<ID3D11Buffer>            m_constantBuffer;
    static inline winrt::com_ptr<ID3D11Buffer>            m_vertexBuffer;
    static inline winrt::com_ptr<ID3D11DepthStencilState> m_depthStencilState;
    static inline winrt::com_ptr<ID3D11BlendState>        m_blendState;
    static inline winrt::com_ptr<ID3D11RasterizerState>   m_rasterizerState;
    static inline winrt::com_ptr<ID3D11SamplerState>      m_samplerState;

    // Intermediate texture for DX12 backbuffer copy
    static inline winrt::com_ptr<ID3D11Texture2D>             m_intermediateTex;
    static inline winrt::com_ptr<ID3D11ShaderResourceView>    m_intermediateSRV;
    static inline UINT m_intermediateWidth = 0;
    static inline UINT m_intermediateHeight = 0;

private:
    static constexpr UINT VERTEX_STRIDE = sizeof(float) * 5;
    static constexpr UINT VERTEX_OFFSET = 0;
};
