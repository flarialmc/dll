#pragma once
#include <d3d11.h>
#include <winrt/base.h>

class RealMotionBlurHelper
{
public:
    static bool Initialize();
    static bool CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);
    static void Render(ID3D11RenderTargetView* rtv, winrt::com_ptr<ID3D11ShaderResourceView>& frame);

    static inline winrt::com_ptr<ID3D11PixelShader>      m_pixelShader;
    static inline winrt::com_ptr<ID3D11VertexShader>     m_vertexShader;
    static inline winrt::com_ptr<ID3D11InputLayout>      m_inputLayout;
    static inline winrt::com_ptr<ID3D11Buffer>           m_constantBuffer;
    static inline winrt::com_ptr<ID3D11Buffer>           m_vertexBuffer;
    static inline winrt::com_ptr<ID3D11DepthStencilState> m_depthStencilState;
    static inline winrt::com_ptr<ID3D11BlendState>        m_blendState;
    static inline winrt::com_ptr<ID3D11RasterizerState>   m_rasterizerState;
    static inline winrt::com_ptr<ID3D11SamplerState>     m_samplerState;
    static inline D3D11_VIEWPORT          m_cachedViewport;
    static inline float                   m_prevWorldMatrix[16];
    
private:
    static constexpr UINT VERTEX_STRIDE = sizeof(float) * 5;
    static constexpr UINT VERTEX_OFFSET = 0;
    static constexpr FLOAT BACKGROUND_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};
