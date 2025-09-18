#include "Engine.hpp"
#include "../../Client.hpp"
#include <utility>
#include <winrt/base.h>
#include <cmath>
#include <imgui/imgui_internal.h>
#include <variant>
#include "Constraints.hpp"
#include "animations/fadeinout.hpp"
#include "Elements/Structs/ImagesClass.hpp"
#include "Elements/Control/Tooltip/ToolTipStruct.hpp"
#include "Elements/Structs/HSV.hpp"
#include "Elements/Structs/Notification.hpp"
#include "Elements/Windows/WindowRect.hpp"
#include "Elements/Control/Slider/SliderRect.hpp"
#include "Elements/Control/TextBox/TextBoxStruct.hpp"
#include "Elements/Control/ColorPicker/ColorPicker.hpp"
#include "Elements/Control/Dropdown/DropdownStruct.hpp"
#include <string>
#include <windows.h>
#include <unknwn.h>
#include <dwrite.h>
#include <wrl.h>
#include <vector>
#include <fstream>
#include <algorithm>

#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "imgui/imgui_freetype.h"
//#include <misc/freetype/imgui_freetype.h>

#include "../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::map<std::string, ID2D1Bitmap*> ImagesClass::eimages;
std::map<int, ID2D1Bitmap*> ImagesClass::images;

std::map<int, ID3D11ShaderResourceView*> ImagesClass::ImguiDX11Images;
std::map<int, ImTextureID> ImagesClass::ImguiDX12Images;
std::map<int, ID3D12Resource*> ImagesClass::ImguiDX12Textures;

// TODO: release it !!!
ID2D1Factory* FlarialGUI::factory;
IDWriteFactory* FlarialGUI::writeFactory;
ID2D1ImageBrush* FlarialGUI::blurbrush;
std::unordered_map<std::string, float> FlarialGUI::TextSizes;
std::unordered_map<std::string, Vec2<float>> FlarialGUI::TextSizesXY;

// Static variables moved from header for better compilation performance
namespace FlarialGUI {
    std::stack<Dimensions> dimensionStack;
    std::vector<float> darkenAmounts;
    std::vector<float> glowAlphas;
    std::vector<float> opacityAmounts;
    std::vector<float> toggleSpacings;
    std::vector<float> rotationAngles;
    std::vector<D2D1_COLOR_F> toggleColors;
    std::vector<D2D1_COLOR_F> buttonColors;

    D2D1_COLOR_F rgbColor;
    float rgbHue = 0.0f;

    float frameFactor = 1;
    bool shouldAdditionalY = false;
    int additionalIndex = 0;
    int highestAddIndexes = 0;
    int activeColorPickerWindows = 0;
    int activeSliders = 0;

    bool isInScrollView = false;
    D2D1_RECT_F ScrollViewRect = D2D1::RectF();

    float scrollpos = 0;
    float scrollposmodifier = 0.f;
    float barscrollpos = 0;
    float barscrollposmodifier = 10.0f;

    std::unordered_map<FontKey, ImFont*> FontMap = {};
    std::unordered_map<FontKey, bool> FontsNotFound = {};

    FontKey LoadFontLater;
    bool DoLoadFontLater;
    bool HasAFontLoaded = false;
    std::vector<std::pair<std::vector<std::byte>, FontKey>> FontMemoryToLoad;

    std::string currentKeybind;

    std::vector<Notification> notifications;

    bool isInWindowRect = false;
    bool inMenu = false;
    bool resizing = false;
    bool needsBackBuffer = false;
    bool hasLoadedAll = false;

    ID2D1Effect* blur = nullptr;
    ID2D1Effect* shadow_blur = nullptr;
    ID2D1Bitmap* screen_bitmap_cache = nullptr;
    ID2D1Image* blur_bitmap_cache = nullptr;

    int maxRect = 0;
}

// Blur class static variables moved from header
winrt::com_ptr<ID3D11PixelShader> Blur::pGaussianBlurHorizontalShader = nullptr;
winrt::com_ptr<ID3D11PixelShader> Blur::pGaussianBlurVerticalShader = nullptr;
winrt::com_ptr<ID3D11VertexShader> Blur::pVertexShader = nullptr;
winrt::com_ptr<ID3D11InputLayout> Blur::pInputLayout = nullptr;

winrt::com_ptr<ID3D11SamplerState> Blur::pSampler = nullptr;
winrt::com_ptr<ID3D11Buffer> Blur::pVertexBuffer = nullptr;
winrt::com_ptr<ID3D11Buffer> Blur::pConstantBuffer = nullptr;
BlurInputBuffer Blur::constantBuffer;

winrt::com_ptr<ID3D11Texture2D> Blur::pIntermediateTexture1 = nullptr;
winrt::com_ptr<ID3D11Texture2D> Blur::pIntermediateTexture2 = nullptr;
winrt::com_ptr<ID3D11RenderTargetView> Blur::pIntermediateRTV1 = nullptr;
winrt::com_ptr<ID3D11RenderTargetView> Blur::pIntermediateRTV2 = nullptr;
winrt::com_ptr<ID3D11ShaderResourceView> Blur::pIntermediateSRV1 = nullptr;
winrt::com_ptr<ID3D11ShaderResourceView> Blur::pIntermediateSRV2 = nullptr;
UINT Blur::currentTextureWidth = 0;
UINT Blur::currentTextureHeight = 0;

winrt::com_ptr<ID3D11DepthStencilState> Blur::pDepthStencilState = nullptr;
winrt::com_ptr<ID3D11BlendState> Blur::pBlendState = nullptr;
winrt::com_ptr<ID3D11RasterizerState> Blur::pRasterizerState = nullptr;

// BlurDX12 class static variables moved from header
winrt::com_ptr<ID3D11PixelShader> BlurDX12::pUpsampleShader = nullptr;
winrt::com_ptr<ID3D11PixelShader> BlurDX12::pDownsampleShader = nullptr;
winrt::com_ptr<ID3D11VertexShader> BlurDX12::pVertexShader = nullptr;
winrt::com_ptr<ID3D11InputLayout> BlurDX12::pInputLayout = nullptr;

winrt::com_ptr<ID3D11SamplerState> BlurDX12::pSampler = nullptr;
winrt::com_ptr<ID3D11Buffer> BlurDX12::pVertexBuffer = nullptr;
winrt::com_ptr<ID3D11Buffer> BlurDX12::pConstantBuffer = nullptr;
BlurInputBuffer BlurDX12::constantBuffer;

// todo: all use cache
std::unordered_map<std::string, ToolTipStruct> FlarialGUI::tooltips;
LRUCache<UINT32, winrt::com_ptr<ID2D1SolidColorBrush>> FlarialGUI::brushCache(300);
std::unordered_map<std::string, ID2D1Image*> FlarialGUI::cachedBitmaps;

LRUCache<uint64_t, winrt::com_ptr<IDWriteTextLayout>> FlarialGUI::textLayoutCache(4000);
LRUCache<UINT32, winrt::com_ptr<IDWriteTextFormat>> FlarialGUI::textFormatCache(300);

LRUCache<std::wstring, std::string> FlarialGUI::fromWideCache(10000);
LRUCache<std::string, std::wstring> FlarialGUI::toWideCache(10000);

std::unordered_map<int, float> FlarialGUI::additionalY;
//std::unordered_map<std::string, winrt::com_ptr<ID2D1GradientStopCollection>> FlarialGUI::gradientStopCache;
LRUCache<uint64_t, winrt::com_ptr<ID2D1LinearGradientBrush>> FlarialGUI::gradientBrushCache(300);

std::unordered_map<int, WindowRect> FlarialGUI::WindowRects;
std::unordered_map<int, SliderRect> FlarialGUI::SliderRects;
std::unordered_map<int, SliderIntRect> FlarialGUI::SliderIntRects;
std::unordered_map<int, TextBoxStruct> FlarialGUI::TextBoxes;
std::unordered_map<int, ColorPicker> FlarialGUI::ColorPickers;
std::unordered_map<int, DropdownStruct> FlarialGUI::DropDownMenus;
std::unordered_map<int, KeybindSelector> FlarialGUI::KeybindSelectors;
std::unordered_map<int, bool> FlarialGUI::ToggleIsHovering;
std::unordered_map<int, bool> FlarialGUI::buttonsHovered;
std::unordered_map<int, bool> FlarialGUI::radioButtonsHovered;

void FlarialGUI::OverrideAlphaValues(float percent) {
	//FlarialGUI::lerp()
	clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value = percent;
}

void FlarialGUI::ResetOverrideAlphaValues() {
	clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value = 1.f;
}

void FlarialGUI::resetColorPicker(size_t index) {
	FlarialGUI::ColorPickers[index].oldHex = "";
	FlarialGUI::ColorPickers[index].oldOpac = 0.0f;
	FlarialGUI::ColorPickers[index].newHex = "";
	FlarialGUI::ColorPickers[index].isActive = false;
	FlarialGUI::ColorPickers[index].hueX = 0.0f;
	FlarialGUI::ColorPickers[index].oldHueX = 0.0f;
	FlarialGUI::ColorPickers[index].movingHueX = false;
	FlarialGUI::ColorPickers[index].opacX = 0.0f;
	FlarialGUI::ColorPickers[index].movingOpacX = false;
	FlarialGUI::ColorPickers[index].shade = Vec2<float>{ -1, -1 };
	FlarialGUI::ColorPickers[index].movingShade = false;

	/*clickgui->color_pickers[index].value = nullptr;
	clickgui->color_pickers[index].opacity = nullptr;
	clickgui->color_pickers[index].rgb = nullptr;*/
}

UINT32 ColorValueToUInt(const D3DCOLORVALUE& color) {
	auto r = static_cast<uint8_t>(color.r * 255.0f);
	auto g = static_cast<uint8_t>(color.g * 255.0f);
	auto b = static_cast<uint8_t>(color.b * 255.0f);
	auto a = static_cast<uint8_t>(color.a * 255.0f);

	// Combine the color components into a single unsigned int
	return (static_cast<UINT32>(a) << 24) | (static_cast<UINT32>(r) << 16) | (static_cast<UINT32>(g) << 8) | b;
}

uint64_t generateUniqueKey(const wchar_t* text, DWRITE_TEXT_ALIGNMENT alignment,
	float fontSize, float weight, float maxWidth,
	float maxHeight) {

	// Use std::hash to create a hash value for each parameter
	std::hash<std::wstring> textHash;
	std::hash<int> alignmentHash;
	std::hash<float> fontSizeHash;
	std::hash<float> weightHash;
	std::hash<float> maxWidthHash;
	std::hash<float> maxHeightHash;

	// Combine the hash values of each parameter
	size_t combinedHash = textHash(text) ^
		alignmentHash(alignment) ^
		fontSizeHash(fontSize) ^
		weightHash(weight) ^
		maxWidthHash(maxWidth) ^
		maxHeightHash(maxHeight);

	return combinedHash;
}

uint32_t generateUniqueTextFormatKey(std::string& font, int alignment,
	float fontSize, float weight) {

	std::hash<std::string> fontHash;
	std::hash<int> alignmentHash;
	std::hash<float> fontSizeHash;
	std::hash<float> weightHash;

	size_t combinedHash = fontHash(font) ^
		alignmentHash(alignment) ^
		fontSizeHash(fontSize) ^
		weightHash(weight);

	return combinedHash;
}

uint64_t generateUniqueLinearGradientBrushKey(float x, float hexPreviewSize, float shadePickerWidth,
	ID2D1GradientStopCollection* pGradientStops) {

	// Get gradient stops
	UINT32 stopCount = pGradientStops->GetGradientStopCount();
	auto gradientStops = std::make_shared<D2D1_GRADIENT_STOP[]>(stopCount);
	pGradientStops->GetGradientStops(gradientStops.get(), stopCount);

	// Hash for gradient stops' colors
	std::hash<UINT32> colorHash;
	uint64_t colorKey = 0;
	for (UINT32 i = 0; i < stopCount; ++i) {
		colorKey ^= colorHash(ColorValueToUInt(gradientStops[i].color));
	}

	// Use std::hash to create a hash value for each parameter
	std::hash<float> xHash;
	std::hash<float> hexPreviewSizeHash;
	std::hash<float> shadePickerWidthHash;
	std::hash<UINT32> stopsHash;

	// Combine the hash values of each parameter
	size_t combinedHash = xHash(x) ^
		hexPreviewSizeHash(hexPreviewSize) ^
		shadePickerWidthHash(shadePickerWidth) ^
		stopsHash(stopCount) ^
		colorKey;

	gradientStops.reset();

	return combinedHash;
}

std::string WideToNarrow_creator(const std::wstring& wideStr) {
	bool isAscii = true;
	for (wchar_t wc : wideStr) {
		if (wc > 127) {
			isAscii = false;
			break;
		}
	}
	if (isAscii) {
		std::string result(wideStr.size(), '\0');
		for (size_t i = 0; i < wideStr.size(); ++i) {
			result[i] = static_cast<char>(wideStr[i]);
		}
		return result;
	}
	int narrowStrLen = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (narrowStrLen <= 0) return {};
	std::string result(narrowStrLen - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &result[0], narrowStrLen, nullptr, nullptr);
	return result;
}

std::string FlarialGUI::WideToNarrow(const std::wstring& wideStr) {
	return fromWideCache.getOrInsert(WideToNarrow_creator, wideStr, wideStr);
}


bool FlarialGUI::CursorInRect(float rectX, float rectY, float width, float height) {
	if (MC::mousePos.x >= rectX && MC::mousePos.x <= rectX + width && MC::mousePos.y >= rectY &&
		MC::mousePos.y <= rectY + height) {
		return true;
	}
	return false;
}

bool FlarialGUI::isRectInRect(const D2D1_RECT_F& outer, const D2D1_RECT_F& inner) {
	return (inner.left <= outer.right &&
		inner.right >= outer.left &&
		inner.top <= outer.bottom &&
		inner.bottom >= outer.top);
}

void FlarialGUI::PushSize(float x, float y, float width, float height) {
	Dimensions size;
	size.x = x;
	size.y = y;
	size.width = width;
	size.height = height;

	dimensionStack.push(size);
}

void FlarialGUI::PopSize() {
	dimensionStack.pop();
}

void FlarialGUI::PopAllStack() {
	if (!dimensionStack.empty()) {
		dimensionStack.pop();
	}
}

winrt::com_ptr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
	ID2D1GradientStopCollection* pGradientStops) {
	winrt::com_ptr<ID2D1LinearGradientBrush> gBrush;

	D2D::context->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize), 0),
			D2D1::Point2F(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize) +
				shadePickerWidth, 0)
		),
		D2D1::BrushProperties(),
		pGradientStops,
		gBrush.put()
	);

	return gBrush;
}

winrt::com_ptr<ID2D1LinearGradientBrush> FlarialGUI::getLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
	ID2D1GradientStopCollection* pGradientStops) {

	uint64_t key = generateUniqueLinearGradientBrushKey(x, hexPreviewSize, shadePickerWidth, pGradientStops);

	winrt::com_ptr<ID2D1LinearGradientBrush> result = FlarialGUI::gradientBrushCache.getOrInsert(CreateLinearGradientBrush, key, x, hexPreviewSize, shadePickerWidth, pGradientStops);
	return result;
}

winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(const wchar_t* text,
	const DWRITE_TEXT_ALIGNMENT alignment,
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, const float fontSize,
	const DWRITE_FONT_WEIGHT weight, float maxWidth, float maxHeight,
	bool moduleFont) {

	winrt::com_ptr<IDWriteTextFormat> textFormat = FlarialGUI::GetTextFormat(alignment, paragraphAlignment, fontSize,
		weight, moduleFont);

	winrt::com_ptr<IDWriteTextLayout> textLayout;
	//IDWriteTextLayout* textLayout;
	FlarialGUI::writeFactory->CreateTextLayout(
		text,
		(UINT32)wcslen(text),
		textFormat.get(),
		maxWidth,
		maxHeight,
		textLayout.put()
	);
	return textLayout;
}

winrt::com_ptr<ID2D1SolidColorBrush> CreateBrush(D2D1_COLOR_F color) {
	winrt::com_ptr<ID2D1SolidColorBrush> brush;

	D2D::context->CreateSolidColorBrush(color, brush.put());

	return brush;
}

winrt::com_ptr<ID2D1SolidColorBrush> FlarialGUI::getBrush(D2D1_COLOR_F color) {

	UINT32 key = ColorValueToUInt(color);

	winrt::com_ptr<ID2D1SolidColorBrush> result = FlarialGUI::brushCache.getOrInsert(CreateBrush, key, color);
	//const winrt::com_ptr<IDWriteTextFormat> result = CreateTextFormat(alignment, paragraphAlignment, fontSize, weight, moduleFont);
	return result;
}

// TODO Constrains to constants !!!
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(const DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
	const float fontSize,
	const DWRITE_FONT_WEIGHT weight, bool moduleFont) {
	std::string fontName = Client::settings.getSettingByName<std::string>(
		moduleFont ? "mod_fontname" : "fontname")->value;

	winrt::com_ptr<IDWriteTextFormat> textFormat;

	FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(fontName).c_str(),
		nullptr,
		weight,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		Constraints::FontScaler(fontSize),
		L"en-us",
		textFormat.put()
	);

	textFormat->SetTextAlignment(alignment);

	textFormat->SetParagraphAlignment(paragraphAlignment);

	return textFormat;
}

winrt::com_ptr<IDWriteTextFormat>  FlarialGUI::GetTextFormat(const DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
	const float fontSize,
	const DWRITE_FONT_WEIGHT weight, bool moduleFont) {
	UINT32 key = generateUniqueTextFormatKey(
		Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value, alignment,
		fontSize, weight);
	winrt::com_ptr<IDWriteTextFormat> result = FlarialGUI::textFormatCache.getOrInsert(CreateTextFormat, key, alignment,
		paragraphAlignment, fontSize,
		weight, moduleFont);
	//const winrt::com_ptr<IDWriteTextFormat> result = CreateTextFormat(alignment, paragraphAlignment, fontSize, weight, moduleFont);
	return result;
}

winrt::com_ptr<IDWriteTextLayout> FlarialGUI::GetTextLayout(const wchar_t* text,
	const DWRITE_TEXT_ALIGNMENT alignment,
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
	const float fontSize,
	const DWRITE_FONT_WEIGHT weight, float maxWidth,
	float maxHeight, bool moduleFont) {
	uint64_t key = generateUniqueKey(text, alignment, fontSize, weight, maxWidth, maxHeight);
	winrt::com_ptr<IDWriteTextLayout> result = FlarialGUI::textLayoutCache.getOrInsert(CreateTextLayout, key, text,
		alignment, paragraphAlignment,
		fontSize, weight, maxWidth,
		maxHeight, moduleFont);
	//const winrt::com_ptr<IDWriteTextLayout>& result = CreateTextLayout(text, alignment, paragraphAlignment, fontSize, weight, maxWidth, maxHeight, moduleFont);
	return result;
}

// Color utility functions moved to EngineColorUtils.cpp for better compilation performance



void FlarialGUI::LoadAllImageToCache() {

	for (int i = 101; i < MAX_IMAGE_ID; ++i) {
		LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
	}

	hasLoadedAll = true;

}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}


bool hasEnding(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}



// Window management, scrolling, text, resource, and utility functions moved to Engine modules for better compilation performance

void FlarialGUI::SetIsInAdditionalYMode() {
	shouldAdditionalY = true;
}

void FlarialGUI::UnSetIsInAdditionalYMode() {
	shouldAdditionalY = false;
}

float FlarialGUI::SettingsTextWidth(const std::string& text) {

	return 100; //what the fuck is this

}
