#pragma once
#include <d3d11.h>
#include <winrt/base.h>

class OnixMotionBlurHelper
{
public:
    static bool Initialize();
    static bool CompileShader(const char* srcData, const char* entryPoint,
                             const char* shaderModel, ID3DBlob** blobOut);
    static winrt::com_ptr<ID3D11ShaderResourceView> Render(
        winrt::com_ptr<ID3D11ShaderResourceView>& inputFrame);
    static void Reset();

private:
    // Shaders and resources
    static inline winrt::com_ptr<ID3D11PixelShader> m_boxBlurShader;
    static inline winrt::com_ptr<ID3D11VertexShader> m_vertexShader;
    static inline winrt::com_ptr<ID3D11InputLayout> m_inputLayout;
    static inline winrt::com_ptr<ID3D11Buffer> m_constantBuffer;
    static inline winrt::com_ptr<ID3D11Buffer> m_vertexBuffer;

    // Render states
    static inline winrt::com_ptr<ID3D11DepthStencilState> m_depthStencilState;
    static inline winrt::com_ptr<ID3D11BlendState> m_blendState;
    static inline winrt::com_ptr<ID3D11RasterizerState> m_rasterizerState;
    static inline winrt::com_ptr<ID3D11SamplerState> m_samplerState;

    // Ping-pong render targets for 12-pass iterative blur
    static inline winrt::com_ptr<ID3D11Texture2D> m_pingTexture;
    static inline winrt::com_ptr<ID3D11Texture2D> m_pongTexture;
    static inline winrt::com_ptr<ID3D11RenderTargetView> m_pingRTV;
    static inline winrt::com_ptr<ID3D11RenderTargetView> m_pongRTV;
    static inline winrt::com_ptr<ID3D11ShaderResourceView> m_pingSRV;
    static inline winrt::com_ptr<ID3D11ShaderResourceView> m_pongSRV;

    // Cached viewport
    static inline D3D11_VIEWPORT m_cachedViewport;
    static inline int m_cachedWidth;
    static inline int m_cachedHeight;

    static bool CreateRenderTargets(int width, int height);
    static void CleanupRenderTargets();

    static constexpr UINT VERTEX_STRIDE = sizeof(float) * 5;
    static constexpr UINT VERTEX_OFFSET = 0;
    static constexpr int NUM_PASSES = 12;  // Fixed 12 passes
};
