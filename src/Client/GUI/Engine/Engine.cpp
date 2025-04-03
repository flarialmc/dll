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
#include "../../../Assets/Assets.hpp"
#include <string>
#include <windows.h>
#include <dwrite.h>
#include <wrl.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "imgui/imgui_freetype.h"
//#include <misc/freetype/imgui_freetype.h>

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_enabled HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_enabled")->value)
#define o_colors_enabled clickgui->settings.getSettingByName<float>("o_colors_enabled")->value
#define colors_enabled_rgb clickgui->settings.getSettingByName<bool>("colors_enabled_rgb")->value

#define colors_disabled HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_disabled")->value)
#define o_colors_disabled clickgui->settings.getSettingByName<float>("o_colors_disabled")->value
#define colors_disabled_rgb clickgui->settings.getSettingByName<bool>("colors_disabled_rgb")->value

#define colors_primary1 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_primary2 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary2")->value)
#define o_colors_primary2 clickgui->settings.getSettingByName<float>("o_colors_primary2")->value
#define colors_primary2_rgb clickgui->settings.getSettingByName<bool>("colors_primary2_rgb")->value

#define colors_primary3 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary3")->value)
#define o_colors_primary3 clickgui->settings.getSettingByName<float>("o_colors_primary3")->value
#define colors_primary3_rgb clickgui->settings.getSettingByName<bool>("colors_primary3_rgb")->value

#define colors_primary4 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary4")->value)
#define o_colors_primary4 clickgui->settings.getSettingByName<float>("o_colors_primary4")->value
#define colors_primary4_rgb clickgui->settings.getSettingByName<bool>("colors_primary4_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary4")->value)
#define o_colors_secondary3 clickgui->settings.getSettingByName<float>("o_colors_secondary3")->value
#define colors_secondary3_rgb clickgui->settings.getSettingByName<bool>("colors_secondary4_rgb")->value

#define colors_secondary4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary3")->value)
#define o_colors_secondary4 clickgui->settings.getSettingByName<float>("o_colors_secondary4")->value
#define colors_secondary4_rgb clickgui->settings.getSettingByName<bool>("colors_secondary3_rgb")->value

#define colors_secondary5 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary5")->value)
#define o_colors_secondary5 clickgui->settings.getSettingByName<float>("o_colors_secondary5")->value
#define colors_secondary5_rgb clickgui->settings.getSettingByName<bool>("colors_secondary5_rgb")->value

#define colors_secondary6 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary6")->value)
#define o_colors_secondary6 clickgui->settings.getSettingByName<float>("o_colors_secondary6")->value
#define colors_secondary6_rgb clickgui->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

#define colors_secondary8 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary8")->value)
#define o_colors_secondary8 clickgui->settings.getSettingByName<float>("o_colors_secondary8")->value
#define colors_secondary8_rgb clickgui->settings.getSettingByName<bool>("colors_secondary8_rgb")->value

#define colors_mod1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod1")->value)
#define o_colors_mod1 clickgui->settings.getSettingByName<float>("o_colors_mod1")->value
#define colors_mod1_rgb clickgui->settings.getSettingByName<bool>("colors_mod1_rgb")->value

#define colors_mod2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod2")->value)
#define o_colors_mod2 clickgui->settings.getSettingByName<float>("o_colors_mod2")->value
#define colors_mod2_rgb clickgui->settings.getSettingByName<bool>("colors_mod2_rgb")->value

#define colors_mod3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod3")->value)
#define o_colors_mod3 clickgui->settings.getSettingByName<float>("o_colors_mod3")->value
#define colors_mod3_rgb clickgui->settings.getSettingByName<bool>("colors_mod3_rgb")->value

#define colors_mod4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod4")->value)
#define o_colors_mod4 clickgui->settings.getSettingByName<float>("o_colors_mod4")->value
#define colors_mod4_rgb clickgui->settings.getSettingByName<bool>("colors_mod4_rgb")->value

std::map<std::string, ID2D1Bitmap *> ImagesClass::eimages;
std::map<int, ID2D1Bitmap *> ImagesClass::images;

std::map<int, ID3D11ShaderResourceView*> ImagesClass::ImguiDX11Images;
std::map<int, ImTextureID> ImagesClass::ImguiDX12Images;

// TODO: release it !!!
ID2D1Factory *FlarialGUI::factory;
IDWriteFactory *FlarialGUI::writeFactory;
ID2D1ImageBrush* FlarialGUI::blurbrush;
std::unordered_map<std::string, float> FlarialGUI::TextSizes;
std::unordered_map<std::string, Vec2<float>> FlarialGUI::TextSizesXY;

// todo: all use cache
std::unordered_map<std::string, ToolTipStruct> FlarialGUI::tooltips;
LRUCache<UINT32, winrt::com_ptr<ID2D1SolidColorBrush>> FlarialGUI::brushCache(300);
std::unordered_map<std::string, ID2D1Image *> FlarialGUI::cachedBitmaps;

LRUCache<uint64_t, winrt::com_ptr<IDWriteTextLayout>> FlarialGUI::textLayoutCache(4000);
LRUCache<UINT32, winrt::com_ptr<IDWriteTextFormat>> FlarialGUI::textFormatCache(300);

LRUCache<std::wstring, std::string> FlarialGUI::fromWideCache(10000);
LRUCache<std::string, std::wstring> FlarialGUI::toWideCache(10000);

std::unordered_map<int, float> FlarialGUI::additionalY;
//std::unordered_map<std::string, winrt::com_ptr<ID2D1GradientStopCollection>> FlarialGUI::gradientStopCache;
LRUCache<uint64_t, winrt::com_ptr<ID2D1LinearGradientBrush>> FlarialGUI::gradientBrushCache(300);

std::unordered_map<int, WindowRect> FlarialGUI::WindowRects;
std::unordered_map<int, SliderRect> FlarialGUI::SliderRects;
std::unordered_map<int, TextBoxStruct> FlarialGUI::TextBoxes;
std::unordered_map<int, ColorPicker> FlarialGUI::ColorPickers;
std::unordered_map<int, DropdownStruct> FlarialGUI::DropDownMenus;
std::unordered_map<int, KeybindSelector> FlarialGUI::KeybindSelectors;

float b_o_colors_text;
float b_o_colors_enabled;
float b_o_colors_disabled;
float b_o_colors_primary1;
float b_o_colors_primary2;
float b_o_colors_primary3;
float b_o_colors_primary4;
float b_o_colors_secondary1;
float b_o_colors_secondary2;
float b_o_colors_secondary3;
float b_o_colors_secondary4;
float b_o_colors_secondary5;
float b_o_colors_secondary6;
float b_o_colors_secondary7;
float b_o_colors_secondary8;
float b_o_colors_mod1;
float b_o_colors_mod2;
float b_o_colors_mod3;
float b_o_colors_mod4;

void FlarialGUI::OverrideAlphaValues(float percent) {
    b_o_colors_text = o_colors_text;
    b_o_colors_enabled = o_colors_enabled;
    b_o_colors_disabled = o_colors_disabled;
    b_o_colors_primary1 = o_colors_primary1;
    b_o_colors_primary2 = o_colors_primary2;
    b_o_colors_primary3 = o_colors_primary3;
    b_o_colors_primary4 = o_colors_primary4;
    b_o_colors_secondary1 = o_colors_secondary1;
    b_o_colors_secondary2 = o_colors_secondary2;
    b_o_colors_secondary3 = o_colors_secondary3;
    b_o_colors_secondary4 = o_colors_secondary4;
    b_o_colors_secondary5 = o_colors_secondary5;
    b_o_colors_secondary6 = o_colors_secondary6;
    b_o_colors_secondary7 = o_colors_secondary7;
    b_o_colors_secondary8 = o_colors_secondary8;
    b_o_colors_mod1 = o_colors_mod1;
    b_o_colors_mod2 = o_colors_mod2;
    b_o_colors_mod3 = o_colors_mod3;
    b_o_colors_mod4 = o_colors_mod4;

    o_colors_text = percent * b_o_colors_text;
    o_colors_enabled = percent * b_o_colors_enabled;
    o_colors_disabled = percent * b_o_colors_disabled;
    o_colors_primary1 = percent * b_o_colors_primary1;
    o_colors_primary2 = percent * b_o_colors_primary2;
    o_colors_primary3 = percent * b_o_colors_primary3;
    o_colors_primary4 = percent * b_o_colors_primary4;
    o_colors_secondary1 = percent * b_o_colors_secondary1;
    o_colors_secondary2 = percent * b_o_colors_secondary2;
    o_colors_secondary3 = percent * b_o_colors_secondary3;
    o_colors_secondary4 = percent * b_o_colors_secondary4;
    o_colors_secondary5 = percent * b_o_colors_secondary5;
    o_colors_secondary6 = percent * b_o_colors_secondary6;
    o_colors_secondary7 = percent * b_o_colors_secondary7;
    o_colors_secondary8 = percent * b_o_colors_secondary8;
    o_colors_mod1 = percent * b_o_colors_mod1;
    o_colors_mod2 = percent * b_o_colors_mod2;
    o_colors_mod3 = percent * b_o_colors_mod3;
    o_colors_mod4 = percent * b_o_colors_mod4;
}

void FlarialGUI::ResetOverrideAlphaValues() {
    o_colors_text = b_o_colors_text;
    o_colors_enabled = b_o_colors_enabled;
    o_colors_disabled = b_o_colors_disabled;
    o_colors_primary1 = b_o_colors_primary1;
    o_colors_primary2 = b_o_colors_primary2;
    o_colors_primary3 = b_o_colors_primary3;
    o_colors_primary4 = b_o_colors_primary4;
    o_colors_secondary1 = b_o_colors_secondary1;
    o_colors_secondary2 = b_o_colors_secondary2;
    o_colors_secondary3 = b_o_colors_secondary3;
    o_colors_secondary4 = b_o_colors_secondary4;
    o_colors_secondary5 = b_o_colors_secondary5;
    o_colors_secondary6 = b_o_colors_secondary6;
    o_colors_secondary7 = b_o_colors_secondary7;
    o_colors_secondary8 = b_o_colors_secondary8;
    o_colors_mod1 = b_o_colors_mod1;
    o_colors_mod2 = b_o_colors_mod2;
    o_colors_mod3 = b_o_colors_mod3;
    o_colors_mod4 = b_o_colors_mod4;
}

UINT32 ColorValueToUInt(const D3DCOLORVALUE &color) {
    auto r = static_cast<uint8_t>(color.r * 255.0f);
    auto g = static_cast<uint8_t>(color.g * 255.0f);
    auto b = static_cast<uint8_t>(color.b * 255.0f);
    auto a = static_cast<uint8_t>(color.a * 255.0f);

    // Combine the color components into a single unsigned int
    return (static_cast<UINT32>(a) << 24) | (static_cast<UINT32>(r) << 16) | (static_cast<UINT32>(g) << 8) | b;
}

uint64_t generateUniqueKey(const wchar_t *text, DWRITE_TEXT_ALIGNMENT alignment,
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

uint32_t generateUniqueTextFormatKey(std::string &font, int alignment,
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

bool FlarialGUI::isRectInRect(const D2D1_RECT_F &outer, const D2D1_RECT_F &inner) {
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
                                                     ID2D1GradientStopCollection *pGradientStops) {
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
                                                                            ID2D1GradientStopCollection *pGradientStops) {

    uint64_t key = generateUniqueLinearGradientBrushKey(x, hexPreviewSize, shadePickerWidth, pGradientStops);

    winrt::com_ptr<ID2D1LinearGradientBrush> result = FlarialGUI::gradientBrushCache.getOrInsert(CreateLinearGradientBrush, key, x, hexPreviewSize, shadePickerWidth, pGradientStops);
    return result;
}

winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(const wchar_t *text,
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
            (UINT32) wcslen(text),
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

winrt::com_ptr<IDWriteTextLayout> FlarialGUI::GetTextLayout(const wchar_t *text,
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

D2D_COLOR_F FlarialGUI::LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float t) {
    if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
        // Interpolate each color channel separately
        float r = color1.r + (color2.r - color1.r) * t;
        float g = color1.g + (color2.g - color1.g) * t;
        float b = color1.b + (color2.b - color1.b) * t;
        float a = color1.a + (color2.a - color1.a) * t;

        return D2D1::ColorF(r, g, b, a);
    } else return color2;
}

void FlarialGUI::HSLToRGB(float h, float s, float l, float &r, float &g, float &b) {
    if (s == 0.0f) {
        r = g = b = l;
    } else {
        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;

        r = HueToRGB(p, q, h + 1.0f / 3.0f);
        g = HueToRGB(p, q, h);
        b = HueToRGB(p, q, h - 1.0f / 3.0f);
    }
}

float FlarialGUI::HueToRGB(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;

    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

    return p;
}

ImColor FlarialGUI::D2DColorToImColor(D2D1_COLOR_F color) {
    return ImColor(color.r, color.g, color.b, color.a);
}

HSV FlarialGUI::RGBtoHSV(D2D1_COLOR_F rgb) {
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;

    HSV out{};
    double min, max, delta;

    min = r < g ? r : g;
    min = min < b ? min : b;

    max = r > g ? r : g;
    max = max > b ? max : b;

    out.value = (float)max;                                // v
    delta = max - min;
    if (delta < 0.00001) {
        out.saturation = 0;
        out.hue = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.saturation = (float)(delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.saturation = 0.0;
        out.hue = NAN;                            // its now undefined
        return out;
    }
    if (r >= max)                           // > is bogus, just keeps compilor happy
        out.hue = (float)((g - b) / delta);        // between yellow & magenta
    else if (g >= max)
        out.hue = 2.0f + (b - r) / delta;  // between cyan & yellow
    else
        out.hue = 4.0f + (r - g) / delta;  // between magenta & cyan

    out.hue *= 60.0;                              // degrees

    if (out.hue < 0.0)
        out.hue += 360.0;

    return out;
}

D2D1::ColorF FlarialGUI::HSVtoColorF(float H, float s, float v) {
    if (H > 360 || H < 0 || s > 1 || s < 0 || v > 1 || v < 0) {
        return {0, 0, 0};
    }
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0f, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    } else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    } else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    } else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    } else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    } else {
        r = C, g = 0, b = X;
    }

    return {r + m, g + m, b + m};
}

D2D1::ColorF FlarialGUI::HexToColorF(const std::string &hexString) {
    if (hexString.length() != 6) {
        // Return black if the string length is not 6
        return {0.0f, 0.0f, 0.0f};
    }

    try {
        // Convert the hex string to individual color components
        uint32_t hex = std::stoul(hexString, nullptr, 16);
        uint8_t red = (hex >> 16) & 0xFF;
        uint8_t green = (hex >> 8) & 0xFF;
        uint8_t blue = hex & 0xFF;

        // Normalize the color components to the range [0.0, 1.0]
        float normalizedRed = static_cast<float>(red) / 255.0f;
        float normalizedGreen = static_cast<float>(green) / 255.0f;
        float normalizedBlue = static_cast<float>(blue) / 255.0f;

        // Return the D2D1::ColorF struct
        return {normalizedRed, normalizedGreen, normalizedBlue};
    }
    catch (const std::exception &) {
        // Return black if the conversion fails
        return {0.0f, 0.0f, 0.0f};
    }
}


std::string FlarialGUI::ColorFToHex(const D2D1_COLOR_F &color) {
    // Convert the color components from the range [0.0, 1.0] to [0, 255]
    auto red = static_cast<uint8_t>(color.r * 255.0f);
    auto green = static_cast<uint8_t>(color.g * 255.0f);
    auto blue = static_cast<uint8_t>(color.b * 255.0f);

    // Combine the color components into a hex string
    char hexString[7];
    sprintf(hexString, "%02x%02x%02x", red, green, blue);

    return {hexString};
}


void FlarialGUI::FlarialText(float x, float y, const wchar_t *text, float width, const float height,
                             const DWRITE_TEXT_ALIGNMENT alignment) {
    D2D1_COLOR_F color = colors_text_rgb ? rgbColor : colors_text;
    color.a = o_colors_text;

    if (isInScrollView) y += scrollpos;

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

    //IDWriteTextFormat* textFormat = FlarialGUI::getTextFormat(Client::settings.getSettingByName<std::string>("fontname")->value, , DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, alignment);
    auto textLayout = FlarialGUI::GetTextLayout(text, alignment, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, width,
                                                DWRITE_FONT_WEIGHT_REGULAR, width, height);

    //D2D1_RECT_F textRect = D2D1::RectF(x, y, x + width, y + height);
    D2D::context->DrawTextLayout(D2D1::Point2F(x, y), textLayout.get(), FlarialGUI::getBrush(color).get());

}

void FlarialGUI::LoadAllImageToCache() {

    for (int i = 101; i < MAX_IMAGE_ID; ++i) {
        LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
    }

    hasLoadedAll = true;

}



bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

DWRITE_FONT_WEIGHT FlarialGUI::GetFontWeightFromString(const std::string& weightStr) {
    if (weightStr == "Bold") {
        return DWRITE_FONT_WEIGHT_BOLD;
    } else if (weightStr == "Normal") {
        return DWRITE_FONT_WEIGHT_NORMAL;
    } else if (weightStr == "SemiBold") {
        return DWRITE_FONT_WEIGHT_SEMI_BOLD;
    } else if (weightStr == "ExtraBold") {
        return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    } else if (weightStr == "Medium") {
        return DWRITE_FONT_WEIGHT_MEDIUM;
    } else if (weightStr == "Light") {
        return DWRITE_FONT_WEIGHT_LIGHT;
    } else if (weightStr == "ExtraLight") {
        return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    } else {
        return DWRITE_FONT_WEIGHT_NORMAL;
    }
}



struct PairHash {
    std::size_t operator()(const std::pair<std::string, DWRITE_FONT_WEIGHT>& p) const {
        return std::hash<std::string>()(p.first) ^ (std::hash<int>()(static_cast<int>(p.second)) << 1);
    }
};

std::string FlarialGUI::GetWeightedName(std::string name, DWRITE_FONT_WEIGHT weight) {
    static std::unordered_map<std::pair<std::string, DWRITE_FONT_WEIGHT>, std::string, PairHash> cache;

    auto key = std::make_pair(name, weight);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
        std::string overrideValue = name + "-" + Client::settings.getSettingByName<std::string>("fontWeight")->value;
        cache[key] = overrideValue;
        return overrideValue;
    }

    std::string weightedName = name;
    if (!name.contains("-")) {
        switch (weight) {
            case DWRITE_FONT_WEIGHT_BOLD: weightedName += "-Bold"; break;
            case DWRITE_FONT_WEIGHT_NORMAL: weightedName += "-Normal"; break;
            case DWRITE_FONT_WEIGHT_SEMI_BOLD: weightedName += "-SemiBold"; break;
            case DWRITE_FONT_WEIGHT_EXTRA_BOLD: weightedName += "-ExtraBold"; break;
            case DWRITE_FONT_WEIGHT_MEDIUM: weightedName += "-Medium"; break;
            case DWRITE_FONT_WEIGHT_LIGHT: weightedName += "-Light"; break;
            case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: weightedName += "-ExtraLight"; break;
            default: weightedName += "-Normal"; break;
        }
    } else {
        weightedName += "-Normal";
    }

    cache[key] = weightedName;
    return weightedName;
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t *text, const float width, const float height,
                                     const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
                                     const DWRITE_FONT_WEIGHT weight, bool moduleFont, bool troll) {


    D2D1_COLOR_F color = colors_text_rgb ? rgbColor : colors_text;

    color.a = o_colors_text;

    if(FlarialGUI::inMenu && !troll && ClickGUI::settingsOpacity != 1 && ClickGUI::curr != "modules" && !ClickGUI::editmenu) color.a = ClickGUI::settingsOpacity;

    return FlarialTextWithFont(x, y, text, width, height, alignment, fontSize, weight, color, moduleFont);
}

std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t *text, const float width, const float height,
                                     const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
                                     const DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color, bool moduleFont) {
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }
    if (isInScrollView) y += scrollpos;
    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return "no";

    std::string font = Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value;

    float guiScale = Client::settings.getSettingByName<float>(moduleFont ? "modules_font_scale" : "gui_font_scale")->value;
    float targetFontSize = (fontSize * guiScale) * 0.18f;

    const std::vector<int> fontSizeBuckets = {16, 32, 64, 128, 256};

    int baseFontSize = fontSizeBuckets.back();
    for (size_t i = 0; i < fontSizeBuckets.size(); i++) {
        if (targetFontSize <= fontSizeBuckets[i]) {
            baseFontSize = fontSizeBuckets[i];
            break;
        }
    }

    float scaleFactor = targetFontSize / static_cast<float>(baseFontSize);

    FontKey fontK = {font, weight, baseFontSize};

    if(fontK.name.contains("minecraft")) fontK.name = "164";
    if(!FontMap[fontK] && fontK.weight == DWRITE_FONT_WEIGHT_NORMAL) fontK.weight = DWRITE_FONT_WEIGHT_MEDIUM;
    if(fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";

    if(!FontMap[fontK] && !FontsNotFound[fontK]) {
        LoadFontLater = fontK;
        DoLoadFontLater = true;
    }

    if (!FontMap[fontK] || font == "Space Grotesk") fontK.name = "162";

    if(!FontMap[fontK] && !FontsNotFound[fontK]) {
        LoadFontLater = fontK;
        DoLoadFontLater = true;
    }

    if (!FontMap[fontK]) return "";
    if (FontMap[fontK]->Scale <= 0.0f || !FontMap[fontK]->IsLoaded()) return "";

    ImGui::PushFont(FontMap[fontK]);
    ImGui::SetWindowFontScale(scaleFactor);

    std::string stringText = WideToNarrow(text);
    ImVec2 size = ImGui::CalcTextSize(stringText.c_str());
    std::string fontedName = fontK.name + cached_to_string(targetFontSize);

    switch (alignment) {
        case DWRITE_TEXT_ALIGNMENT_LEADING:
            break;
        case DWRITE_TEXT_ALIGNMENT_CENTER: {
            x += (width / 2) - (size.x / 2);
            break;
        }
        case DWRITE_TEXT_ALIGNMENT_TRAILING: {
            x += (width - size.x);
            break;
        }
        default: ;
    }

    TextSizes[fontedName] = size.x;
    TextSizesXY[fontedName] = Vec2<float>(size.x, size.y);
    y += (height / 2) - (size.y / 2);

    ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImColor(color.r, color.g, color.b, color.a), stringText.c_str());

    ImGui::SetWindowFontScale(1.0);
    ImGui::PopFont();

    return fontedName;
}

void FlarialGUI::ExtractImageResource(int resourceId, std::string fileName, LPCTSTR type) {
    LPVOID pFileData = NULL;
    DWORD dwFileSize = 0;

    HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (hRes == NULL)
        return;

    HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
    if (hResData == NULL)
        return;

    pFileData = LockResource(hResData);
    if (pFileData == NULL)
        return;

    dwFileSize = SizeofResource(Client::currentModule, hRes);

    std::string fileType(type);

    std::filesystem::path path(fmt::format("{}\\{}", Utils::getAssetsPath(), fileName));

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile) {
        return;
    }
    // Write the file data directly as binary
    outFile.write(reinterpret_cast<const char*>(pFileData), dwFileSize);
    outFile.close();
}

void FlarialGUI::LoadFont(int resourceId) {
    LPVOID pFontData = NULL;
    DWORD dwFontSize = 0;

    HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (hRes == NULL)
        return;

    HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
    if (hResData == NULL)
        return;

    pFontData = LockResource(hResData);
    if (pFontData == NULL)
        return;

    dwFontSize = SizeofResource(Client::currentModule, hRes);

    std::string lpFileName = fmt::format("{}\\{}.ttf", Utils::getAssetsPath(), resourceId);

    if(std::filesystem::exists(lpFileName)) return;

    std::ofstream outFile(lpFileName, std::ios::binary);
    if (!outFile) {
        return;
    }

    // Write the font data directly as binary
    outFile.write(reinterpret_cast<const char*>(pFontData), dwFontSize);
    outFile.close();

    //AddFontResource(lpFileName.c_str());
}

std::wstring FlarialGUI::GetFontFilePath(const std::wstring& fontName, DWRITE_FONT_WEIGHT weight) {
    Microsoft::WRL::ComPtr<IDWriteFactory> factory;
    Microsoft::WRL::ComPtr<IDWriteFontCollection> fontCollection;
    Microsoft::WRL::ComPtr<IDWriteFontFamily> fontFamily;
    Microsoft::WRL::ComPtr<IDWriteFont> font;
    Microsoft::WRL::ComPtr<IDWriteFontFace> fontFace;
    Microsoft::WRL::ComPtr<IDWriteFontFile> fontFile;
    const void* fontFileReferenceKey;
    UINT32 fontFileReferenceKeySize;
    Microsoft::WRL::ComPtr<IDWriteFontFileLoader> fontFileLoader;
    Microsoft::WRL::ComPtr<IDWriteLocalFontFileLoader> localFontFileLoader;
    UINT32 filePathLength = 0;

    // Create a DirectWrite factory
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &factory);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the system font collection
    hr = factory->GetSystemFontCollection(&fontCollection);
    if (FAILED(hr)) {
        return L"";
    }

    // Find the font family
    UINT32 index = 0;
    BOOL exists = FALSE;
    hr = fontCollection->FindFamilyName(fontName.c_str(), &index, &exists);
    if (FAILED(hr) || !exists) {
        return L"";
    }

    // Get the font family
    hr = fontCollection->GetFontFamily(index, &fontFamily);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the font count in the family
    UINT32 fontCount = fontFamily->GetFontCount();
    if (fontCount == 0) {
        return L"";
    }

    // Find the font with the specified weight
    UINT32 fontIndex = -1;
    for (UINT32 i = 0; i < fontCount; ++i) {
        Microsoft::WRL::ComPtr<IDWriteFont> tempFont;
        hr = fontFamily->GetFont(i, &tempFont);
        if (FAILED(hr)) {
            continue;
        }
        if (tempFont->GetWeight() == weight) {

            fontIndex = i;
            fontFamily->GetFont(fontIndex, &font);
            break;
        }
    }


    if (fontIndex == -1) {
        if(FAILED(fontFamily->GetFont(0, &font))) return L"";
    }

    // Get the font face
    hr = font->CreateFontFace(&fontFace);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the font file count
    UINT32 fileCount = 0;
    hr = fontFace->GetFiles(&fileCount, nullptr);
    if (FAILED(hr) || fileCount == 0) {
        return L"";
    }

    // Allocate space for the font files
    std::vector<Microsoft::WRL::ComPtr<IDWriteFontFile>> fontFiles(fileCount);

    // Get the font files
    hr = fontFace->GetFiles(&fileCount, reinterpret_cast<IDWriteFontFile**>(fontFiles.data()));
    if (FAILED(hr)) {
        return L"";
    }

    // Get the file reference key
    hr = fontFiles[0]->GetReferenceKey(&fontFileReferenceKey, &fontFileReferenceKeySize);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the font file loader
    hr = fontFiles[0]->GetLoader(&fontFileLoader);
    if (FAILED(hr)) {
        return L"";
    }

    // Query for the local font file loader interface
    hr = fontFileLoader->QueryInterface(__uuidof(IDWriteLocalFontFileLoader), &localFontFileLoader);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the font file path length
    hr = localFontFileLoader->GetFilePathLengthFromKey(fontFileReferenceKey, fontFileReferenceKeySize, &filePathLength);
    if (FAILED(hr)) {
        return L"";
    }

    // Get the font file path
    std::vector<wchar_t> filePathBuffer(filePathLength + 1);
    hr = localFontFileLoader->GetFilePathFromKey(fontFileReferenceKey, fontFileReferenceKeySize, filePathBuffer.data(), filePathLength + 1);
    if (FAILED(hr)) {
        return L"";
    }

    return std::wstring(filePathBuffer.data(), filePathLength);
}

void FlarialGUI::queueFontMemoryLoad(std::wstring filepath, FontKey fontK) {
    std::string tral = WideToNarrow(filepath);
    std::thread([tral, filepath, fontK]() {
        std::ifstream fontFile(tral, std::ios::binary);
        if (fontFile.is_open()) {
            Logger::debug("Path {}", tral);
            std::vector<std::byte> fontData = Memory::readFile(filepath);
            fontFile.close();
            FontMemoryToLoad.push_back(std::pair(fontData, fontK));
        }
    }).detach();
}

bool FlarialGUI::LoadFontFromFontFamily(FontKey fontK) {

    if (!FlarialGUI::FontMemoryToLoad.empty()) {
        for (auto it = FlarialGUI::FontMemoryToLoad.begin(); it != FlarialGUI::FontMemoryToLoad.end(); ) {
            Logger::debug("Loading font: {}", fontK.name);
            ImFontConfig config;
            config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_MonoHinting;
            config.FontDataOwnedByAtlas = false;
            FontMap[it->second] = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(it->first.data(), static_cast<int>(it->first.size()), it->second.size, &config,ImGui::GetIO().Fonts->GetGlyphRangesDefault());
            it = FlarialGUI::FontMemoryToLoad.erase(it);
            HasAFontLoaded = true;
        }
    }

    std::string name = fontK.name;
    std::transform(name.begin(), name.end(), name.begin(), ::towlower);
    std::wstring fontName = FlarialGUI::to_wide(name);
    std::wstring fontFilePath = GetFontFilePath(fontName, fontK.weight);
    std::string path;

    if (fontK.name == "162") {
        path = Utils::getAssetsPath() + "\\162" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    } else if (fontK.name == "163") {
        path = Utils::getAssetsPath() + "\\163" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    } else if (fontK.name == "164") {
        path = Utils::getAssetsPath() + "\\164" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    }

    if (!fontFilePath.empty()) {

        std::ifstream fontFile(fontFilePath, std::ios::binary);
        if (fontFile.is_open()) {

            //FontMap[fontK] = ImGui::GetIO().Fonts->AddFontFromFileTTF(WideToNarrow(fontFilePath).c_str(), fontK.size, &config);
            queueFontMemoryLoad(fontFilePath, fontK);

            return true;
        }
    }

    FontsNotFound[fontK] = true;
    return false;
}


void FlarialGUI::LoadImageFromResource(int resourceId, ID2D1Bitmap **bitmap, LPCTSTR type) {
    IWICBitmapDecoder *pDecoder = nullptr;
    IWICBitmapFrameDecode *pFrame = nullptr;
    IWICStream *pStream = nullptr;
    HRSRC imageResHandle = nullptr;
    HGLOBAL imageResDataHandle = nullptr;
    void *pImageFile = nullptr;
    DWORD imageFileSize = 0;

    // Locate the resource
    imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);

    // Load the resource
    imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);

    // Lock the resource to get a pointer to the image data
    pImageFile = LockResource(imageResDataHandle);
    imageFileSize = SizeofResource(Client::currentModule, imageResHandle);

    IWICImagingFactory *imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

    // Create a WIC stream to map onto the memory
    imagingFactory->CreateStream(&pStream);

    // Initialize the stream with the memory pointer and size
    pStream->InitializeFromMemory(static_cast<BYTE *>(pImageFile), imageFileSize);

    // Create a decoder for the stream
    imagingFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);

    // Get the first frame of the image
    pDecoder->GetFrame(0, &pFrame);

    // Convert the frame to a Direct2D bitmap
    IWICFormatConverter *pConverter = NULL;
    imagingFactory->CreateFormatConverter(&pConverter);
    pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0,
                           WICBitmapPaletteTypeMedianCut);

    D2D::context->CreateBitmapFromWicBitmap(pConverter, nullptr, bitmap);

    if (pConverter)
        pConverter->Release();
    if (pDecoder)
        pDecoder->Release();
    if (pFrame)
        pFrame->Release();
    if (pStream)
        pStream->Release();
}

void FlarialGUI::LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap) {
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    IWICBitmapDecoder *bitmapDecoder = nullptr;
    IWICBitmapFrameDecode *frame = nullptr;
    IWICFormatConverter *formatConverter = nullptr;

    IWICImagingFactory *imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

    // Create decoder
    imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
                                              &bitmapDecoder);

    // Get first frame
    bitmapDecoder->GetFrame(0, &frame);

    // Convert format to 32bppPBGRA
    imagingFactory->CreateFormatConverter(&formatConverter);
    formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0,
                                WICBitmapPaletteTypeMedianCut);

    // Create bitmap
    D2D::context->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);

    bitmapDecoder->Release();
    frame->Release();
    formatConverter->Release();
    imagingFactory->Release();
}

void FlarialGUI::SetScrollView(float x, float y, float width, float height) {
    FlarialGUI::isInScrollView = true;
    D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
    ScrollViewRect = clipRect;
    PushImClipRect(clipRect);
    D2D::context->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

}

void FlarialGUI::UnsetScrollView() {
    FlarialGUI::isInScrollView = false;
    PopImClipRect();
    D2D::context->PopAxisAlignedClip();
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum, float fixer) {
    isInWindowRect = true;

    D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::White);
    c.a = 0.5f;
    FlarialGUI::RoundedRect(x, y, c, width, height, 0, 0);
    c.a = 1.f;
    FlarialGUI::RoundedHollowRect(x, y, Constraints::RelativeConstraint(0.003f, "height", true), c, width, height, 0, 0);

    if(currentNum > maxRect) maxRect = currentNum;

    int i = 0;
    bool ye = false;
    for (const auto& [key, rect] : WindowRects) {
        if (rect.isMovingElement && key != currentNum) {
            ye = true;
            break;
        }
    }

    WindowRects[currentNum].width = width;
    WindowRects[currentNum].height = height;

    if (!ye) {

        if ((CursorInRect(x, y, width, height) || WindowRects[currentNum].isMovingElement) && MC::held) {

            if (!WindowRects[currentNum].isMovingElement) {
                WindowRects[currentNum].oriMouseX = (MC::mousePos.x - ((x + width / 2.0f)));
                WindowRects[currentNum].oriMouseY = (MC::mousePos.y - ((y + height / 2.0f)));
                WindowRects[currentNum].isMovingElement = true;
            }
            WindowRects[currentNum].hasBeenMoved = true;
            WindowRects[currentNum].movedX =
                    ((MC::mousePos.x - WindowRects[currentNum].oriMouseX) - width / 2.0f) + fixer;
            WindowRects[currentNum].movedY = ((MC::mousePos.y - WindowRects[currentNum].oriMouseY) - height / 2.0f);

        }

        if (MC::mouseButton == MouseButton::None && !MC::held || MC::mouseButton == MouseButton::Left && !MC::held) {
            WindowRects[currentNum].isMovingElement = false;
            WindowRects[currentNum].oriMouseX = -1;
            WindowRects[currentNum].oriMouseY = -1;
        }
    }


    // Check if outside of screen and constrain window
    WindowRects[currentNum].fixer = fixer;
    if (WindowRects[currentNum].movedX - fixer < 0) WindowRects[currentNum].movedX = 0.001f + fixer;
    if (WindowRects[currentNum].movedY < 0) WindowRects[currentNum].movedY = 0;

    if (WindowRects[currentNum].movedX + width - fixer > MC::windowSize.x)
        WindowRects[currentNum].movedX = MC::windowSize.x - width + fixer;
    if (WindowRects[currentNum].movedY + height > MC::windowSize.y)
        WindowRects[currentNum].movedY = MC::windowSize.y - height;

    WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
    WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;

    if (WindowRects[currentNum].isMovingElement && Client::settings.getSettingByName<bool>("snappinglines")->value) {
        const float alignmentThreshold = 10.0f;
        const ImColor pink(1.0f, 0.0f, 1.0f, 1.0f);


        // test release lol
        for (int j = 0; j <= maxRect; j++) {
            if (j == currentNum) continue;

            auto it = WindowRects.find(j);
            if (it != WindowRects.end()) {
                auto& otherRect = it->second;

                // Snap to the left edge of another rectangle
                if (fabs(WindowRects[currentNum].movedX - otherRect.movedX - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX, 0), ImVec2(otherRect.movedX, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + fixer;
                }

                // Snap to the right edge of another rectangle
                if (fabs(WindowRects[currentNum].movedX + width - (otherRect.movedX + otherRect.width) - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width, 0), ImVec2(otherRect.movedX + otherRect.width, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width - width + fixer;
                }

                // Snap to the top edge of another rectangle
                if (fabs(WindowRects[currentNum].movedY - otherRect.movedY) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY), ImVec2(MC::windowSize.x, otherRect.movedY), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY;
                }

                // Snap to the bottom edge of another rectangle
                if (fabs(WindowRects[currentNum].movedY + height - (otherRect.movedY + otherRect.height)) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height - height;
                }

                // Snap to the center alignment with another rectangle horizontally
                if (fabs((WindowRects[currentNum].movedX + width / 2.0f) - (otherRect.movedX + otherRect.width / 2.0f) - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width / 2.0f, 0), ImVec2(otherRect.movedX + otherRect.width / 2.0f, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width / 2.0f - width / 2.0f + fixer;
                }

                // Snap to the center alignment with another rectangle vertically (no `fixer` on y-axis)
                if (fabs((WindowRects[currentNum].movedY + height / 2.0f) - (otherRect.movedY + otherRect.height / 2.0f)) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height / 2.0f), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height / 2.0f), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height / 2.0f - height / 2.0f;
                }
            }
        }

        // Snap to the screen edges and center alignment with `fixer` adjustments on x-axis only
        if (fabs(WindowRects[currentNum].movedX - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX, 0),
                                                    ImVec2(WindowRects[currentNum].movedX, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = fixer; // Snap to left edge with `fixer`
        }

        if (fabs(WindowRects[currentNum].movedX + width - MC::windowSize.x - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX + width, 0),
                                                    ImVec2(WindowRects[currentNum].movedX + width, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = MC::windowSize.x - width + fixer; // Snap to right edge with `fixer`
        }

        // Snap to the vertical edges without fixer
        if (fabs(WindowRects[currentNum].movedY) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY),
                                                    ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY), pink, 2.0f);
            WindowRects[currentNum].movedY = 0.0f;
        }

        if (fabs(WindowRects[currentNum].movedY + height - MC::windowSize.y) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY + height),
                                                    ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY + height), pink, 2.0f);
            WindowRects[currentNum].movedY = MC::windowSize.y - height;
        }

        // Center align horizontally with fixer applied
        if (fabs((WindowRects[currentNum].movedX + width / 2.0f - MC::windowSize.x / 2.0f) - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(MC::windowSize.x / 2.0f, 0),
                                                    ImVec2(MC::windowSize.x / 2.0f, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = MC::windowSize.x / 2.0f - width / 2.0f + fixer;
        }

        // Center align vertically without fixer
        if (fabs(WindowRects[currentNum].movedY + height / 2.0f - MC::windowSize.y / 2.0f) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, MC::windowSize.y / 2.0f),
                                                    ImVec2(MC::windowSize.x, MC::windowSize.y / 2.0f), pink, 2.0f);
            WindowRects[currentNum].movedY = MC::windowSize.y / 2.0f - height / 2.0f;
        }
    }
}


void FlarialGUI::UnsetWindowRect() {
    isInWindowRect = false;
}

Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num, float rectWidth, float rectHeight) {
    if (isInWindowRect && WindowRects[num].hasBeenMoved) {

        WindowRects[num].percentageX = WindowRects[num].movedX / MC::windowSize.x;
        WindowRects[num].percentageY = WindowRects[num].movedY / MC::windowSize.y;

        x = (WindowRects[num].percentageX * MC::windowSize.x);
        y = (WindowRects[num].percentageY * MC::windowSize.y);

    }

    if (x - WindowRects[num].fixer < 0) x = 0.001f - WindowRects[num].fixer;
    if (y < 0) y = 0;

    if (x + rectWidth - WindowRects[num].fixer > MC::windowSize.x)
        x = MC::windowSize.x - rectWidth + WindowRects[num].fixer;
    if (y + rectHeight > MC::windowSize.y) y = MC::windowSize.y - rectHeight;

    if(x == 0) x = 0.0001f;
    if(y == 0) y = 0.0001f;


    return Vec2<float>{x, y};
}

Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight) {
    Vec2<float> xy;
    xy.x = (D2D::context->GetSize().width - rectWidth) / 2.0f;
    xy.y = (D2D::context->GetSize().height - rectHeight) / 2.0f;
    return xy;
}

void FlarialGUI::ResetShit() {
    // Reset the variables to their initial values or desired values here
    for (auto &i: WindowRects) {
        i.second = WindowRect();
    }

    for (auto &i: SliderRects) {
        i.second = SliderRect();
    }

    for (auto &i: TextBoxes) {
        i.second = TextBoxStruct();
    }

    for (auto &i: ColorPickers) {
        i.second = ::ColorPicker();
    }

    for (auto &i: DropDownMenus) {
        i.second = DropdownStruct();
    }

    activeColorPickerWindows = 0;

}

/* rotation stuff */

static int rotation_start_index;
void FlarialGUI::ImRotateStart()
{
    rotation_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
}

ImVec2 FlarialGUI::ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x+u.x)/2, (l.y+u.y)/2); // or use _ClipRectStack?
}

ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }

void FlarialGUI::ImRotateEnd(float angle, ImVec2 center)
{
    float rad = angle * IM_PI / 180.0f;  // Convert angle from degrees to radians
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

/* rotation stuff end */

void FlarialGUI::PushImClipRect(ImVec2 pos, ImVec2 size) {
    ImVec2 max(pos.x + size.x, pos.y + size.y);

    ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);

}

void FlarialGUI::PushImClipRect(D2D_RECT_F rect) {
    ImVec2 pos(rect.left, rect.top);
    ImVec2 size(rect.right - rect.left, rect.bottom - rect.top);

    ImVec2 max(pos.x + size.x, pos.y + size.y);

    ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);

}

void FlarialGUI::PopImClipRect() {
    ImGui::GetBackgroundDrawList()->PopClipRect();
}

void FlarialGUI::Notify(const std::string& text) {

    if(SwapchainHook::init) {
        Notification e;
        e.text = text;
        e.finished = false;
        e.currentPosY = MC::windowSize.y;
        e.arrived = false;
        e.finished = false;
        notifications.push_back(e);
    }

}
void FlarialGUI::NotifyHeartbeat() {

    // if i dont do this i get a unresolved externals error????
    float funnyTroll = 1.f;
    FlarialGUI::lerp(funnyTroll, 40.f, 0.12f * FlarialGUI::frameFactor);
    // end of torll


    Vec2<float> round = Constraints::RoundingConstraint(20, 20);
    float height = Constraints::RelativeConstraint(0.035, "height", true);
    float posxModif = 0;
    float fontSize = Constraints::RelativeConstraint(0.128, "height", true);
    float textposyModif = Constraints::RelativeConstraint(0.0045f, "height", true);
    D2D1::ColorF col = colors_primary1;

    int i = 0;
    for (Notification& n : notifications) {


        float posyModif = -((height + Constraints::RelativeConstraint(0.01f, "height", true)) * i);

        if (n.firstTime) {
            float TrollSize = Constraints::RelativeConstraint(0.128, "height", true);
            std::string sizeName = FlarialGUI::FlarialTextWithFont(n.currentPos, n.currentPosY, FlarialGUI::to_wide(n.text).c_str(), 10, 25, DWRITE_TEXT_ALIGNMENT_CENTER, TrollSize, DWRITE_FONT_WEIGHT_NORMAL, D2D1::ColorF(0, 0, 0, 0));
            if (FlarialGUI::TextSizes[sizeName] != 0) {
                n.width = FlarialGUI::TextSizes[sizeName] + Constraints::RelativeConstraint(0.0345f, "height", true);
                n.currentPos = Constraints::CenterConstraint(n.width, 0).x;
                n.firstTime = false;
            }
        }

        if (!n.finished) {
            if (!n.arrived) {

                FlarialGUI::RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
                FlarialGUI::FlarialTextWithFont(n.currentPos + posxModif, n.currentPosY + posyModif, FlarialGUI::to_wide(n.text).c_str(), n.width, height, DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);
                FlarialGUI::lerp(n.currentPosY, Constraints::PercentageConstraint(0.1f, "bottom", true), FlarialGUI::frameFactor * 0.067f);

                if (n.currentPosY <= Constraints::PercentageConstraint(0.08f, "bottom", true)) {
                    n.arrived = true;
                    n.time = std::chrono::steady_clock::now();
                }

                i++;

            }
            else {

                std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
                auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - n.time);

                FlarialGUI::RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
                FlarialGUI::FlarialTextWithFont(n.currentPos + posxModif, n.currentPosY + posyModif, FlarialGUI::to_wide(n.text).c_str(), n.width, height, DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);

                if (timeDifference.count() > 5000) {
                    FlarialGUI::lerp(n.currentPosY, MC::windowSize.y + 500, FlarialGUI::frameFactor * 0.052f);
                    if (n.currentPosY >= MC::windowSize.y) { n.finished = true; }
                }

                i++;
            }
        }
        else {
            FlarialGUI::notifications.erase(std::next(FlarialGUI::notifications.begin(), i));
        }
    }
}

/*void FlarialGUI::NotifyHeartbeat() {                                                        //feel free to finish this whenever, i didnt get blur rect to work so it looked ass
    float rectHeight = Constraints::RelativeConstraint(0.09, "height", true);
    float y = Constraints::PercentageConstraint(0.25, "bottom", true);
    Vec2<float> rounding = Constraints::RoundingConstraint(20, 20);

    int i = 0;

    for (auto& notif : FlarialGUI::notifications) {
        float rectWidth = notif.width;Constraints::PercentageConstraint(0.3, "right", true);
        D2D1::ColorF col = colors_secondary3;
        col.a = 0.4f;

        if (notif.firstTime) {
             //   this fixes a bug where a part of the text would be off the screen on smaller screen sizes
             //  idk how to explain it but
             //   it just works
            float TrollSize = Constraints::RelativeConstraint(0.128, "height", true);
            std::string sizeName = FlarialGUI::FlarialTextWithFont(notif.currentPos, notif.currentPosY, FlarialGUI::to_wide(notif.text + "troll troll troll troll troll").c_str(), 10, 25, DWRITE_TEXT_ALIGNMENT_CENTER, TrollSize, DWRITE_FONT_WEIGHT_NORMAL, D2D1::ColorF(0, 0, 0, 0));
            if (FlarialGUI::TextSizes[sizeName] != 0 && Constraints::PercentageConstraint(0.3, "right", true) > MC::windowSize.x - FlarialGUI::TextSizes[sizeName]) {
                notif.currentPos = Constraints::PercentageConstraint(0.4, "right", true);
                notif.firstTime = false;
            } else notif.currentPos = Constraints::PercentageConstraint(0.3, "right", true);
            
            // end of wierd ahh method

            notif.width = MC::windowSize.x; // js have it be as long as possible for it to go off the screen
        }
        if (!notif.arrived) {

            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY, col, notif.width, rectHeight, rounding.x, rounding.y);
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY, colors_primary1_rgb ? FlarialGUI::rgbColor : colors_primary1, rounding.x + 1.0f, rectHeight, 0.0f, 0.0f);

            D2D::context->PopAxisAlignedClip();
            FlarialGUI::PushSize(notif.currentPos, notif.currentPosY, notif.width, rectHeight);

            float logoX = Constraints::PercentageConstraint(0.01, "left") - Constraints::SpacingConstraint(0.18, rectHeight);
            float logoY = Constraints::PercentageConstraint(0.01, "top") - Constraints::SpacingConstraint(0.10, rectHeight);
            float logoWidth = Constraints::RelativeConstraint(1.25);

            FlarialGUI::image(IDR_LOGO_PNG, D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            logoX += Constraints::SpacingConstraint(0.85, logoWidth);
            logoY -= Constraints::SpacingConstraint(0.105, logoWidth);

            FlarialGUI::FlarialTextWithFont(
                logoX,
                logoY,
                L"Notification",
                notif.width,
                logoWidth,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(0.45, Constraints::RelativeConstraint(0.45, "height", true)),
                DWRITE_FONT_WEIGHT_BOLD
            );

            logoY += Constraints::SpacingConstraint(0.185, logoWidth);

            FlarialGUI::FlarialTextWithFont(
                logoX,
                logoY,
                FlarialGUI::to_wide(notif.text).c_str(),
                notif.width, logoWidth,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(0.3, Constraints::RelativeConstraint(0.45, "height", true)),
                DWRITE_FONT_WEIGHT_NORMAL
            );

            Logger::debug("{}", MC::mousePos.y);

            FlarialGUI::PopSize();
            FlarialGUI::lerp(notif.currentPosY, y, 0.20f * FlarialGUI::frameFactor);

            if (abs(notif.currentPosY - y) < 10.0f) {
                Logger::debug("arrived");
                notif.arrived = true;
                notif.time = std::chrono::steady_clock::now();
            }

            i++;
        } else {
            //std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

            //auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - notif.time);

            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY, col, notif.width, rectHeight, rounding.x, rounding.x);
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY, colors_primary1_rgb ? FlarialGUI::rgbColor : colors_primary1, rounding.x + 1.0f, rectHeight, 0.0f, 0.0f);

            D2D::context->PopAxisAlignedClip();
            FlarialGUI::PushSize(notif.currentPos, notif.currentPosY, notif.width, rectHeight);

            float logoX = Constraints::PercentageConstraint(0.01, "left") - Constraints::SpacingConstraint(0.18, rectHeight);
            float logoY = Constraints::PercentageConstraint(0.01, "top") - Constraints::SpacingConstraint(0.10, rectHeight);
            float logoWidth = Constraints::RelativeConstraint(1.25);

            FlarialGUI::image(IDR_LOGO_PNG, D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            logoX += Constraints::SpacingConstraint(0.85, logoWidth);
            logoY -= Constraints::SpacingConstraint(0.105, logoWidth);

            FlarialGUI::FlarialTextWithFont(logoX, logoY, L"Notification",
                notif.width, logoWidth,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(0.45,
                    Constraints::RelativeConstraint(0.45, "height", true)),
                DWRITE_FONT_WEIGHT_BOLD
            );

            logoY += Constraints::SpacingConstraint(0.185, logoWidth);
            FlarialGUI::FlarialTextWithFont(logoX, logoY,
                FlarialGUI::to_wide(notif.text).c_str(),
                notif.width, logoWidth,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(0.3,
                    Constraints::RelativeConstraint(0.45, "height", true)),
                DWRITE_FONT_WEIGHT_NORMAL
            );

            FlarialGUI::PopSize();

            // Fade out animation
            //if (timeDifference.count() > 2000) {
                FlarialGUI::lerp(notif.currentPos, Constraints::PercentageConstraint(0.01, "right", true) + 50, 0.12f * FlarialGUI::frameFactor);
            //}

            if (notif.currentPos > Constraints::PercentageConstraint(0.01, "right", true)) notif.finished = true;
        }

        y -= Constraints::SpacingConstraint(1.25, rectHeight);
        if (notif.finished) FlarialGUI::notifications.erase(std::next(FlarialGUI::notifications.begin(), i));
    }
}*/

void FlarialGUI::CopyBitmap(ID2D1Bitmap1 *from, ID2D1Bitmap **to) {
    if (from == nullptr) {
        Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap",  "From is nullptr");
        return;
    }

    if (*to == nullptr) {
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr)) {
            Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "From is nullptr");
            return;
        }
    } else if (from->GetPixelSize() != (*to)->GetPixelSize()) {
        (*to)->Release();
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap",  "From is nullptr");
            return;
        }
    }

    D2D1_POINT_2U destPoint = {0, 0};
    D2D1_SIZE_U size = from->GetPixelSize();
    D2D1_RECT_U rect = {0, 0, size.width, size.height};
    (*to)->CopyFromBitmap(&destPoint, from, &rect);
}

std::wstring to_wide_creator(const std::string &str) {

    int wchars_num = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wide;
    wide.resize(wchars_num);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wide[0], wchars_num);
    return wide;
}

std::wstring FlarialGUI::to_wide(const std::string &str) {
    return toWideCache.getOrInsert(to_wide_creator, str, str);
}

template<typename T>
void FlarialGUI::lerp(T &a, const T &b, float t) {
    if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
        // Perform linear interpolation between a and b based on t
        float interpolatedValue = a + (b - a) * t;

        // Round up the interpolated value to three decimal places
        float roundedValue = std::ceilf(interpolatedValue * 1000.0f) / 1000.0f;

        // Assign the rounded value back to 'a'
        a = roundedValue;
    } else a = b;
}

void FlarialGUI::SetIsInAdditionalYMode() {
    shouldAdditionalY = true;
}

void FlarialGUI::UnSetIsInAdditionalYMode() {
    shouldAdditionalY = false;
}

float FlarialGUI::SettingsTextWidth(const std::string& text) {

return 100; //what the fuck is this

}