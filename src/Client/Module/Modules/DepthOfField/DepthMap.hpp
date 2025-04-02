#pragma once
#include <d3d11.h>
#include <winrt/base.h>
#include <string>
#include <vector>

class DepthMapGenerator {
public:
    static bool Initialize();
    static bool GenerateDepthMap(ID3D11Texture2D* depthTexture);
    static void Cleanup();

private:
    static bool CompileShader(const char* srcData, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);

    static ID3D11VertexShader* m_vertexShader;
    static ID3D11PixelShader* m_pixelShader;
    static ID3D11InputLayout* m_inputLayout;
    static ID3D11Buffer* m_vertexBuffer;
    static ID3D11RenderTargetView* m_renderTargetView;
    static ID3D11Texture2D* m_renderTexture;
    static ID3D11SamplerState* m_samplerState;
};