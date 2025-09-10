#pragma once

#include "EngineCore.hpp"
#include "../D2D.hpp"
#include <wincodec.h>
#include <winrt/base.h>
#include "../../../Utils/Memory/LRUCache.hpp"

// DirectX-specific includes
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>

using namespace DirectX;

struct BlurInputBuffer
{
	XMFLOAT2 resolution;
	XMFLOAT2 offset;
	XMFLOAT2 halfpixel;
	float intensity;
	float _padding;
};

class Blur
{
public:
	static winrt::com_ptr<ID3D11PixelShader> pGaussianBlurHorizontalShader;
	static winrt::com_ptr<ID3D11PixelShader> pGaussianBlurVerticalShader;
	static winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	static winrt::com_ptr<ID3D11InputLayout> pInputLayout;

	static winrt::com_ptr<ID3D11SamplerState> pSampler;
	static winrt::com_ptr<ID3D11Buffer> pVertexBuffer;
	static winrt::com_ptr<ID3D11Buffer> pConstantBuffer;
	static BlurInputBuffer constantBuffer;

	// Persistent intermediate textures
	static winrt::com_ptr<ID3D11Texture2D> pIntermediateTexture1;
	static winrt::com_ptr<ID3D11Texture2D> pIntermediateTexture2;
	static winrt::com_ptr<ID3D11RenderTargetView> pIntermediateRTV1;
	static winrt::com_ptr<ID3D11RenderTargetView> pIntermediateRTV2;
	static winrt::com_ptr<ID3D11ShaderResourceView> pIntermediateSRV1;
	static winrt::com_ptr<ID3D11ShaderResourceView> pIntermediateSRV2;
	static UINT currentTextureWidth;
	static UINT currentTextureHeight;

	// Cached render states
	static winrt::com_ptr<ID3D11DepthStencilState> pDepthStencilState;
	static winrt::com_ptr<ID3D11BlendState> pBlendState;
	static winrt::com_ptr<ID3D11RasterizerState> pRasterizerState;

	// RAII
	static void InitializePipeline();
	static void Cleanup();

	static void RenderToRTV(ID3D11RenderTargetView*, ID3D11ShaderResourceView*, XMFLOAT2);
	static void RenderBlur(ID3D11RenderTargetView*, int, float);
	static void ReleaseIntermediateTextures();
private:
	static bool EnsureIntermediateTextures(UINT width, UINT height);
};

class BlurDX12
{
public:
	static winrt::com_ptr<ID3D11PixelShader> pUpsampleShader;
	static winrt::com_ptr<ID3D11PixelShader> pDownsampleShader;
	static winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	static winrt::com_ptr<ID3D11InputLayout> pInputLayout;

	static winrt::com_ptr<ID3D11SamplerState> pSampler;
	static winrt::com_ptr<ID3D11Buffer> pVertexBuffer;
	static winrt::com_ptr<ID3D11Buffer> pConstantBuffer;
	static BlurInputBuffer constantBuffer;

	// RAII
	static void InitializePipeline();

	static void RenderBlur(ID3D12GraphicsCommandList* commandList);
};

namespace FlarialGUI {
	extern LRUCache<UINT32, winrt::com_ptr<ID2D1SolidColorBrush>> brushCache;
	extern LRUCache<uint64_t, winrt::com_ptr<IDWriteTextLayout>> textLayoutCache;
	extern LRUCache<UINT32, winrt::com_ptr<IDWriteTextFormat>> textFormatCache;
	extern LRUCache<std::wstring, std::string> fromWideCache;
	extern LRUCache<std::string, std::wstring> toWideCache;
	extern LRUCache<uint64_t, winrt::com_ptr<ID2D1LinearGradientBrush>> gradientBrushCache;

	// DirectX-specific text functions
	winrt::com_ptr<IDWriteTextFormat>
		GetTextFormat(DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
			float fontSize,
			DWRITE_FONT_WEIGHT weight, bool moduleFont);

	winrt::com_ptr<IDWriteTextLayout> GetTextLayout(const wchar_t* text,
		DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		float fontSize = 14,
		DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
		float maxWidth = 500, float maxHeight = 500,
		bool moduleFont = false);

	// DirectX-specific rendering functions
	void ApplyGaussianBlur();
	void ApplyPaintEffect(float blurIntensity);
	void ApplyAestheticDeepFry();
	void ApplyHue(float Hue);
	void ApplyDeepFry(float intensity);
	void ApplyCombinedDeepFry();
	void CopyBitmap(ID2D1Bitmap1* from, ID2D1Bitmap** to);
	void ApplyGaussianBlurToTarget(ID2D1BitmapRenderTarget* target, float blurIntensity);
	void ApplySusGaussianBlur(float blurIntensity);
	void AllahBlur(float intensity);
	void BlurRect(D2D1_ROUNDED_RECT rect);
	void PrepareBlur(float intensity);

	// DirectX brush utilities
	winrt::com_ptr<ID2D1SolidColorBrush> getBrush(D2D1_COLOR_F color);
	winrt::com_ptr<ID2D1LinearGradientBrush> getLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
		ID2D1GradientStopCollection* pGradientStops);

	// DirectX image loading
	void LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap);
	void LoadImageFromResource(int resourceId, ID2D1Bitmap** bitmap, LPCTSTR type = "PNG");
	bool LoadImageFromResource(int resourceId, ID3D11ShaderResourceView** out_srv, LPCTSTR type);
	bool LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type);
}
