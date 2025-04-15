#include "Engine.hpp"
#include "../../Client.hpp"
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
#include <vector>
#include <fstream>
#include <algorithm>
#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "imgui/imgui_freetype.h"

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

std::map<std::string, ID2D1Bitmap*> ImagesClass::eimages;
std::map<int, ID2D1Bitmap*> ImagesClass::images;
std::map<int, ID3D11ShaderResourceView*> ImagesClass::ImguiDX11Images;
std::map<int, ImTextureID> ImagesClass::ImguiDX12Images;

ID2D1Factory* FlarialGUI::factory;
IDWriteFactory* FlarialGUI::writeFactory;
ID2D1ImageBrush* FlarialGUI::blurbrush;
std::unordered_map<std::string, float> FlarialGUI::TextSizes;
std::unordered_map<std::string, Vec2<float>> FlarialGUI::TextSizesXY;
std::unordered_map<std::string, ToolTipStruct> FlarialGUI::tooltips;
LRUCache<UINT32, winrt::com_ptr<ID2D1SolidColorBrush>> FlarialGUI::brushCache(300);
std::unordered_map<std::string, ID2D1Image*> FlarialGUI::cachedBitmaps;
LRUCache<uint64_t, winrt::com_ptr<IDWriteTextLayout>> FlarialGUI::textLayoutCache(4000);
LRUCache<UINT32, winrt::com_ptr<IDWriteTextFormat>> FlarialGUI::textFormatCache(300);
LRUCache<std::wstring, std::string> FlarialGUI::fromWideCache(10000);
LRUCache<std::string, std::wstring> FlarialGUI::toWideCache(10000);
std::unordered_map<int, float> FlarialGUI::additionalY;
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

/**
 * @brief Overrides the alpha values of all colors by a percentage
 * @param percent Percentage to scale the original alpha values (0.0 to 1.0)
 */
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

/**
 * @brief Resets the alpha values of all colors to their original values
 */
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

/**
 * @brief Converts a D3DCOLORVALUE to an unsigned 32-bit integer
 * @param color Color value to convert
 * @return UINT32 representation of the color
 */
UINT32 ColorValueToUInt(const D3DCOLORVALUE& color) {
    auto r = static_cast<uint8_t>(color.r * 255.0f);
    auto g = static_cast<uint8_t>(color.g * 255.0f);
    auto b = static_cast<uint8_t>(color.b * 255.0f);
    auto a = static_cast<uint8_t>(color.a * 255.0f);
    return (static_cast<UINT32>(a) << 24) | (static_cast<UINT32>(r) << 16) | (static_cast<UINT32>(g) << 8) | b;
}

/**
 * @brief Generates a unique key for text layout caching
 * @param text Text string
 * @param alignment Text alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param maxWidth Maximum width of the text layout
 * @param maxHeight Maximum height of the text layout
 * @return Unique 64-bit key
 */
uint64_t generateUniqueKey(const wchar_t* text, DWRITE_TEXT_ALIGNMENT alignment,
    float fontSize, float weight, float maxWidth, float maxHeight) {
    std::hash<std::wstring> textHash;
    std::hash<int> alignmentHash;
    std::hash<float> fontSizeHash;
    std::hash<float> weightHash;
    std::hash<float> maxWidthHash;
    std::hash<float> maxHeightHash;
    size_t combinedHash = textHash(text) ^ alignmentHash(alignment) ^ fontSizeHash(fontSize) ^
        weightHash(weight) ^ maxWidthHash(maxWidth) ^ maxHeightHash(maxHeight);
    return combinedHash;
}

/**
 * @brief Generates a unique key for text format caching
 * @param font Font name
 * @param alignment Text alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @return Unique 32-bit key
 */
uint32_t generateUniqueTextFormatKey(std::string& font, int alignment, float fontSize, float weight) {
    std::hash<std::string> fontHash;
    std::hash<int> alignmentHash;
    std::hash<float> fontSizeHash;
    std::hash<float> weightHash;
    size_t combinedHash = fontHash(font) ^ alignmentHash(alignment) ^ fontSizeHash(fontSize) ^ weightHash(weight);
    return combinedHash;
}

/**
 * @brief Generates a unique key for linear gradient brush caching
 * @param x X-coordinate of the gradient start
 * @param hexPreviewSize Size of the hex preview
 * @param shadePickerWidth Width of the shade picker
 * @param pGradientStops Pointer to gradient stops
 * @return Unique 64-bit key
 */
uint64_t generateUniqueLinearGradientBrushKey(float x, float hexPreviewSize, float shadePickerWidth,
    ID2D1GradientStopCollection* pGradientStops) {
    UINT32 stopCount = pGradientStops->GetGradientStopCount();
    auto gradientStops = std::make_shared<D2D1_GRADIENT_STOP[]>(stopCount);
    pGradientStops->GetGradientStops(gradientStops.get(), stopCount);
    std::hash<UINT32> colorHash;
    uint64_t colorKey = 0;
    for (UINT32 i = 0; i < stopCount; ++i) {
        colorKey ^= colorHash(ColorValueToUInt(gradientStops[i].color));
    }
    std::hash<float> xHash;
    std::hash<float> hexPreviewSizeHash;
    std::hash<float> shadePickerWidthHash;
    std::hash<UINT32> stopsHash;
    size_t combinedHash = xHash(x) ^ hexPreviewSizeHash(hexPreviewSize) ^ shadePickerWidthHash(shadePickerWidth) ^
        stopsHash(stopCount) ^ colorKey;
    gradientStops.reset();
    return combinedHash;
}

/**
 * @brief Converts a wide string to a narrow string without caching
 * @param wideStr Wide string to convert
 * @return Converted narrow string
 */
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

/**
 * @brief Converts a wide string to a narrow string using a cache
 * @param wideStr Wide string to convert
 * @return Converted narrow string
 */
std::string FlarialGUI::WideToNarrow(const std::wstring& wideStr) {
    return fromWideCache.getOrInsert(WideToNarrow_creator, wideStr, wideStr);
}

/**
 * @brief Checks if the cursor is within a rectangular area
 * @param rectX X-coordinate of the rectangle
 * @param rectY Y-coordinate of the rectangle
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @return True if the cursor is within the rectangle, false otherwise
 */
bool FlarialGUI::CursorInRect(float rectX, float rectY, float width, float height) {
    return MC::mousePos.x >= rectX && MC::mousePos.x <= rectX + width &&
        MC::mousePos.y >= rectY && MC::mousePos.y <= rectY + height;
}

/**
 * @brief Checks if one rectangle is within another
 * @param outer Outer rectangle
 * @param inner Inner rectangle
 * @return True if the inner rectangle is within the outer rectangle, false otherwise
 */
bool FlarialGUI::isRectInRect(const D2D1_RECT_F& outer, const D2D1_RECT_F& inner) {
    return inner.left <= outer.right && inner.right >= outer.left &&
        inner.top <= outer.bottom && inner.bottom >= outer.top;
}

/**
 * @brief Pushes a size onto the dimension stack
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param width Width
 * @param height Height
 */
void FlarialGUI::PushSize(float x, float y, float width, float height) {
    Dimensions size;
    size.x = x;
    size.y = y;
    size.width = width;
    size.height = height;
    dimensionStack.push(size);
}

/**
 * @brief Pops the top size from the dimension stack
 */
void FlarialGUI::PopSize() {
    dimensionStack.pop();
}

/**
 * @brief Pops all sizes from the dimension stack if not empty
 */
void FlarialGUI::PopAllStack() {
    if (!dimensionStack.empty()) {
        dimensionStack.pop();
    }
}

/**
 * @brief Creates a linear gradient brush
 * @param x X-coordinate of the gradient start
 * @param hexPreviewSize Size of the hex preview
 * @param shadePickerWidth Width of the shade picker
 * @param pGradientStops Pointer to gradient stops
 * @return Pointer to the created linear gradient brush
 */
winrt::com_ptr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
    ID2D1GradientStopCollection* pGradientStops) {
    winrt::com_ptr<ID2D1LinearGradientBrush> gBrush;
    D2D::context->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize), 0),
            D2D1::Point2F(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize) + shadePickerWidth, 0)
        ),
        D2D1::BrushProperties(),
        pGradientStops,
        gBrush.put()
    );
    return gBrush;
}

/**
 * @brief Retrieves or creates a cached linear gradient brush
 * @param x X-coordinate of the gradient start
 * @param hexPreviewSize Size of the hex preview
 * @param shadePickerWidth Width of the shade picker
 * @param pGradientStops Pointer to gradient stops
 * @return Pointer to the linear gradient brush
 */
winrt::com_ptr<ID2D1LinearGradientBrush> FlarialGUI::getLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
    ID2D1GradientStopCollection* pGradientStops) {
    uint64_t key = generateUniqueLinearGradientBrushKey(x, hexPreviewSize, shadePickerWidth, pGradientStops);
    return gradientBrushCache.getOrInsert(CreateLinearGradientBrush, key, x, hexPreviewSize, shadePickerWidth, pGradientStops);
}

/**
 * @brief Creates a text layout
 * @param text Text string
 * @param alignment Text alignment
 * @param paragraphAlignment Paragraph alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param maxWidth Maximum width
 * @param maxHeight Maximum height
 * @param moduleFont Use module font if true
 * @return Pointer to the created text layout
 */
winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(const wchar_t* text, const DWRITE_TEXT_ALIGNMENT alignment,
    DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, const float fontSize,
    const DWRITE_FONT_WEIGHT weight, float maxWidth, float maxHeight,
    bool moduleFont) {
    winrt::com_ptr<IDWriteTextFormat> textFormat = FlarialGUI::GetTextFormat(alignment, paragraphAlignment, fontSize, weight, moduleFont);
    winrt::com_ptr<IDWriteTextLayout> textLayout;
    FlarialGUI::writeFactory->CreateTextLayout(text, (UINT32)wcslen(text), textFormat.get(), maxWidth, maxHeight, textLayout.put());
    return textLayout;
}

/**
 * @brief Creates a solid color brush
 * @param color Color to create the brush with
 * @return Pointer to the created brush
 */
winrt::com_ptr<ID2D1SolidColorBrush> CreateBrush(D2D1_COLOR_F color) {
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    D2D::context->CreateSolidColorBrush(color, brush.put());
    return brush;
}

/**
 * @brief Retrieves or creates a cached solid color brush
 * @param color Color to create the brush with
 * @return Pointer to the brush
 */
winrt::com_ptr<ID2D1SolidColorBrush> FlarialGUI::getBrush(D2D1_COLOR_F color) {
    UINT32 key = ColorValueToUInt(color);
    return brushCache.getOrInsert(CreateBrush, key, color);
}

/**
 * @brief Creates a text format
 * @param alignment Text alignment
 * @param paragraphAlignment Paragraph alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param moduleFont Use module font if true
 * @return Pointer to the created text format
 */
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(const DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
    const float fontSize, const DWRITE_FONT_WEIGHT weight, bool moduleFont) {
    std::string fontName = Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value;
    winrt::com_ptr<IDWriteTextFormat> textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(fontName).c_str(), nullptr, weight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        Constraints::FontScaler(fontSize), L"en-us", textFormat.put());
    textFormat->SetTextAlignment(alignment);
    textFormat->SetParagraphAlignment(paragraphAlignment);
    return textFormat;
}

/**
 * @brief Retrieves or creates a cached text format
 * @param alignment Text alignment
 * @param paragraphAlignment Paragraph alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param moduleFont Use module font if true
 * @return Pointer to the text format
 */
winrt::com_ptr<IDWriteTextFormat> FlarialGUI::GetTextFormat(const DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
    const float fontSize, const DWRITE_FONT_WEIGHT weight, bool moduleFont) {
    UINT32 key = generateUniqueTextFormatKey(Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value,
        alignment, fontSize, weight);
    return textFormatCache.getOrInsert(CreateTextFormat, key, alignment, paragraphAlignment, fontSize, weight, moduleFont);
}

/**
 * @brief Retrieves or creates a cached text layout
 * @param text Text string
 * @param alignment Text alignment
 * @param paragraphAlignment Paragraph alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param maxWidth Maximum width
 * @param maxHeight Maximum height
 * @param moduleFont Use module font if true
 * @return Pointer to the text layout
 */
winrt::com_ptr<IDWriteTextLayout> FlarialGUI::GetTextLayout(const wchar_t* text, const DWRITE_TEXT_ALIGNMENT alignment,
    DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, const float fontSize,
    const DWRITE_FONT_WEIGHT weight, float maxWidth, float maxHeight, bool moduleFont) {
    uint64_t key = generateUniqueKey(text, alignment, fontSize, weight, maxWidth, maxHeight);
    return textLayoutCache.getOrInsert(CreateTextLayout, key, text, alignment, paragraphAlignment, fontSize, weight, maxWidth, maxHeight, moduleFont);
}

/**
 * @brief Linearly interpolates between two colors
 * @param color1 Starting color
 * @param color2 Ending color
 * @param t Interpolation factor (0.0 to 1.0)
 * @return Interpolated color
 */
D2D_COLOR_F FlarialGUI::LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float t) {
    if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
        float r = color1.r + (color2.r - color1.r) * t;
        float g = color1.g + (color2.g - color1.g) * t;
        float b = color1.b + (color2.b - color1.b) * t;
        float a = color1.a + (color2.a - color1.a) * t;
        return D2D1::ColorF(r, g, b, a);
    }
    return color2;
}

/**
 * @brief Converts HSL color values to RGB
 * @param h Hue (0.0 to 1.0)
 * @param s Saturation (0.0 to 1.0)
 * @param l Lightness (0.0 to 1.0)
 * @param r Red component output (0.0 to 1.0)
 * @param g Green component output (0.0 to 1.0)
 * @param b Blue component output (0.0 to 1.0)
 */
void FlarialGUI::HSLToRGB(float h, float s, float l, float& r, float& g, float& b) {
    if (s == 0.0f) {
        r = g = b = l;
    }
    else {
        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        r = HueToRGB(p, q, h + 1.0f / 3.0f);
        g = HueToRGB(p, q, h);
        b = HueToRGB(p, q, h - 1.0f / 3.0f);
    }
}

/**
 * @brief Helper function to convert hue to RGB component
 * @param p Temporary value p
 * @param q Temporary value q
 * @param t Hue component
 * @return RGB component value
 */
float FlarialGUI::HueToRGB(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

/**
 * @brief Converts a D2D color to an ImGui color
 * @param color D2D color to convert
 * @return ImGui color
 */
ImColor FlarialGUI::D2DColorToImColor(D2D1_COLOR_F color) {
    return ImColor(color.r, color.g, color.b, color.a);
}

/**
 * @brief Converts an RGB color to HSV
 * @param rgb RGB color to convert
 * @return HSV color
 */
HSV FlarialGUI::RGBtoHSV(D2D1_COLOR_F rgb) {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    HSV out{};
    double min = std::min(std::min(r, g), b);
    double max = std::max(std::max(r, g), b);
    out.value = static_cast<float>(max);
    double delta = max - min;
    if (delta < 0.00001) {
        out.saturation = 0;
        out.hue = 0;
        return out;
    }
    if (max > 0.0) {
        out.saturation = static_cast<float>(delta / max);
    }
    else {
        out.saturation = 0.0;
        out.hue = NAN;
        return out;
    }
    if (r >= max) out.hue = static_cast<float>((g - b) / delta);
    else if (g >= max) out.hue = 2.0f + (b - r) / delta;
    else out.hue = 4.0f + (r - g) / delta;
    out.hue *= 60.0f;
    if (out.hue < 0.0) out.hue += 360.0f;
    return out;
}

/**
 * @brief Converts HSV color values to a D2D color
 * @param H Hue (0 to 360)
 * @param s Saturation (0.0 to 1.0)
 * @param v Value (0.0 to 1.0)
 * @return D2D color
 */
D2D1::ColorF FlarialGUI::HSVtoColorF(float H, float s, float v) {
    if (H > 360 || H < 0 || s > 1 || s < 0 || v > 1 || v < 0) return { 0, 0, 0 };
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0f, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) { r = C; g = X; b = 0; }
    else if (H >= 60 && H < 120) { r = X; g = C; b = 0; }
    else if (H >= 120 && H < 180) { r = 0; g = C; b = X; }
    else if (H >= 180 && H < 240) { r = 0; g = X; b = C; }
    else if (H >= 240 && H < 300) { r = X; g = 0; b = C; }
    else { r = C; g = 0; b = X; }
    return { r + m, g + m, b + m };
}

/**
 * @brief Converts a hex string to a D2D color
 * @param hexString 6-character hex color string (e.g., "FF0000")
 * @return D2D color
 */
D2D1::ColorF FlarialGUI::HexToColorF(const std::string& hexString) {
    if (hexString.length() != 6) return { 0.0f, 0.0f, 0.0f };
    try {
        uint32_t hex = std::stoul(hexString, nullptr, 16);
        uint8_t red = (hex >> 16) & 0xFF;
        uint8_t green = (hex >> 8) & 0xFF;
        uint8_t blue = hex & 0xFF;
        return { static_cast<float>(red) / 255.0f, static_cast<float>(green) / 255.0f, static_cast<float>(blue) / 255.0f };
    }
    catch (const std::exception&) {
        return { 0.0f, 0.0f, 0.0f };
    }
}

/**
 * @brief Converts a D2D color to a hex string
 * @param color D2D color to convert
 * @return 6-character hex color string
 */
std::string FlarialGUI::ColorFToHex(const D2D1_COLOR_F& color) {
    auto red = static_cast<uint8_t>(color.r * 255.0f);
    auto green = static_cast<uint8_t>(color.g * 255.0f);
    auto blue = static_cast<uint8_t>(color.b * 255.0f);
    char hexString[7];
    sprintf(hexString, "%02x%02x%02x", red, green, blue);
    return { hexString };
}

/**
 * @brief Renders text using Direct2D
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param text Text to render
 * @param width Width of the text area
 * @param height Height of the text area
 * @param alignment Text alignment
 */
void FlarialGUI::FlarialText(float x, float y, const wchar_t* text, float width, const float height,
    const DWRITE_TEXT_ALIGNMENT alignment) {
    D2D1_COLOR_F color = colors_text_rgb ? rgbColor : colors_text;
    color.a = o_colors_text;
    if (isInScrollView) y += scrollpos;
    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;
    auto textLayout = GetTextLayout(text, alignment, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, width, DWRITE_FONT_WEIGHT_REGULAR, width, height);
    D2D::context->DrawTextLayout(D2D1::Point2F(x, y), textLayout.get(), getBrush(color).get());
}

/**
 * @brief Loads all images into the DX11 image cache
 */
void FlarialGUI::LoadAllImageToCache() {
    for (int i = 101; i < MAX_IMAGE_ID; ++i) {
        LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
    }
    hasLoadedAll = true;
}

/**
 * @brief Maps a font weight string to a DWRITE_FONT_WEIGHT enum
 * @param weightStr Font weight string (e.g., "Bold")
 * @return Corresponding DWRITE_FONT_WEIGHT value
 */
DWRITE_FONT_WEIGHT FlarialGUI::GetFontWeightFromString(const std::string& weightStr) {
    if (weightStr == "Bold") return DWRITE_FONT_WEIGHT_BOLD;
    if (weightStr == "Normal") return DWRITE_FONT_WEIGHT_NORMAL;
    if (weightStr == "SemiBold") return DWRITE_FONT_WEIGHT_SEMI_BOLD;
    if (weightStr == "ExtraBold") return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    if (weightStr == "Medium") return DWRITE_FONT_WEIGHT_MEDIUM;
    if (weightStr == "Light") return DWRITE_FONT_WEIGHT_LIGHT;
    if (weightStr == "ExtraLight") return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    return DWRITE_FONT_WEIGHT_NORMAL;
}

struct PairHash {
    std::size_t operator()(const std::pair<std::string, DWRITE_FONT_WEIGHT>& p) const {
        return std::hash<std::string>()(p.first) ^ (std::hash<int>()(static_cast<int>(p.second)) << 1);
    }
};

/**
 * @brief Generates a weighted font name
 * @param name Base font name
 * @param weight Font weight
 * @return Weighted font name string
 */
std::string FlarialGUI::GetWeightedName(std::string name, DWRITE_FONT_WEIGHT weight) {
    static std::unordered_map<std::pair<std::string, DWRITE_FONT_WEIGHT>, std::string, PairHash> cache;
    auto key = std::make_pair(name, weight);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
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
    }
    else {
        weightedName += "-Normal";
    }
    cache[key] = weightedName;
    return weightedName;
}

/**
 * @brief Renders text with a specified font and default color
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param text Text to render
 * @param width Width of the text area
 * @param height Height of the text area
 * @param alignment Text alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param moduleFont Use module font if true
 * @param troll Flag for special handling
 * @return Font identifier string
 */
std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t* text, const float width, const float height,
    const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
    const DWRITE_FONT_WEIGHT weight, bool moduleFont, bool troll) {
    D2D1_COLOR_F color = colors_text_rgb ? rgbColor : colors_text;
    color.a = o_colors_text;
    if (inMenu && !troll && ClickGUI::settingsOpacity != 1 && ClickGUI::curr != "modules" && !ClickGUI::editmenu) color.a = ClickGUI::settingsOpacity;
    return FlarialTextWithFont(x, y, text, width, height, alignment, fontSize, weight, color, moduleFont);
}

/**
 * @brief Renders text with a specified font and color
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param text Text to render
 * @param width Width of the text area
 * @param height Height of the text area
 * @param alignment Text alignment
 * @param fontSize Font size
 * @param weight Font weight
 * @param color Color of the text
 * @param moduleFont Use module font if true
 * @return Font identifier string
 */
std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t* text, const float width, const float height,
    const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
    const DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color, bool moduleFont) {
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) y += additionalY[i];
        }
    }
    if (isInScrollView) y += scrollpos;
    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return "no";
    std::string font = Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value;
    float guiScale = Client::settings.getSettingByName<float>(moduleFont ? "modules_font_scale" : "gui_font_scale")->value;
    float targetFontSize = (fontSize * guiScale) * 0.18f;
    const std::vector<int> fontSizeBuckets = { 16, 32, 64, 128, 256 };
    int baseFontSize = fontSizeBuckets.back();
    for (size_t i = 0; i < fontSizeBuckets.size(); i++) {
        if (targetFontSize <= fontSizeBuckets[i]) {
            baseFontSize = fontSizeBuckets[i];
            break;
        }
    }
    float scaleFactor = targetFontSize / static_cast<float>(baseFontSize);
    FontKey fontK = { font, weight, baseFontSize };
    if (fontK.name.contains("minecraft")) fontK.name = "164";
    if (!FontMap[fontK] && fontK.weight == DWRITE_FONT_WEIGHT_NORMAL) fontK.weight = DWRITE_FONT_WEIGHT_MEDIUM;
    if (fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";
    if (!FontMap[fontK] && !FontsNotFound[fontK]) {
        LoadFontLater = fontK;
        DoLoadFontLater = true;
    }
    if (!FontMap[fontK] || font == "Space Grotesk") fontK.name = "162";
    if (!FontMap[fontK] && !FontsNotFound[fontK]) {
        LoadFontLater = fontK;
        DoLoadFontLater = true;
    }
    if (!FontMap[fontK] || FontMap[fontK]->Scale <= 0.0f || !FontMap[fontK]->IsLoaded()) return "";
    ImGui::PushFont(FontMap[fontK]);
    ImGui::SetWindowFontScale(scaleFactor);
    std::string stringText = WideToNarrow(text);
    ImVec2 size = ImGui::CalcTextSize(stringText.c_str());
    std::string fontedName = fontK.name + cached_to_string(targetFontSize);
    switch (alignment) {
    case DWRITE_TEXT_ALIGNMENT_CENTER: x += (width / 2) - (size.x / 2); break;
    case DWRITE_TEXT_ALIGNMENT_TRAILING: x += (width - size.x); break;
    default: break;
    }
    TextSizes[fontedName] = size.x;
    TextSizesXY[fontedName] = Vec2<float>(size.x, size.y);
    y += (height / 2) - (size.y / 2);
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImColor(color.r, color.g, color.b, color.a), stringText.c_str());
    ImGui::SetWindowFontScale(1.0);
    ImGui::PopFont();
    return fontedName;
}

/**
 * @brief Extracts an image resource to a file
 * @param resourceId Resource identifier
 * @param fileName Output file name
 * @param type Resource type (e.g., "PNG")
 */
void FlarialGUI::ExtractImageResource(int resourceId, std::string fileName, LPCTSTR type) {
    HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (!hRes) return;
    HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
    if (!hResData) return;
    LPVOID pFileData = LockResource(hResData);
    if (!pFileData) return;
    DWORD dwFileSize = SizeofResource(Client::currentModule, hRes);
    std::filesystem::path path(fmt::format("{}\\{}", Utils::getAssetsPath(), fileName));
    std::ofstream outFile(path, std::ios::binary);
    if (!outFile) return;
    outFile.write(reinterpret_cast<const char*>(pFileData), dwFileSize);
    outFile.close();
}

/**
 * @brief Loads a font from a resource to a file
 * @param resourceId Resource identifier
 */
void FlarialGUI::LoadFont(int resourceId) {
    HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRes) return;
    HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
    if (!hResData) return;
    LPVOID pFontData = LockResource(hResData);
    if (!pFontData) return;
    DWORD dwFontSize = SizeofResource(Client::currentModule, hRes);
    std::string lpFileName = fmt::format("{}\\{}.ttf", Utils::getAssetsPath(), resourceId);
    if (std::filesystem::exists(lpFileName)) return;
    std::ofstream outFile(lpFileName, std::ios::binary);
    if (!outFile) return;
    outFile.write(reinterpret_cast<const char*>(pFontData), dwFontSize);
    outFile.close();
}

/**
 * @brief Retrieves the file path of a font from the system font collection
 * @param fontName Name of the font
 * @param weight Font weight
 * @return File path as a wide string
 */
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
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &factory)) ||
        FAILED(factory->GetSystemFontCollection(&fontCollection))) return L"";
    UINT32 index = 0;
    BOOL exists = FALSE;
    if (FAILED(fontCollection->FindFamilyName(fontName.c_str(), &index, &exists)) || !exists ||
        FAILED(fontCollection->GetFontFamily(index, &fontFamily))) return L"";
    UINT32 fontCount = fontFamily->GetFontCount();
    if (fontCount == 0) return L"";
    UINT32 fontIndex = -1;
    for (UINT32 i = 0; i < fontCount; ++i) {
        Microsoft::WRL::ComPtr<IDWriteFont> tempFont;
        if (FAILED(fontFamily->GetFont(i, &tempFont))) continue;
        if (tempFont->GetWeight() == weight) {
            fontIndex = i;
            fontFamily->GetFont(fontIndex, &font);
            break;
        }
    }
    if (fontIndex == -1 && FAILED(fontFamily->GetFont(0, &font))) return L"";
    if (FAILED(font->CreateFontFace(&fontFace))) return L"";
    UINT32 fileCount = 0;
    if (FAILED(fontFace->GetFiles(&fileCount, nullptr)) || fileCount == 0) return L"";
    std::vector<Microsoft::WRL::ComPtr<IDWriteFontFile>> fontFiles(fileCount);
    if (FAILED(fontFace->GetFiles(&fileCount, reinterpret_cast<IDWriteFontFile**>(fontFiles.data())))) return L"";
    if (FAILED(fontFiles[0]->GetReferenceKey(&fontFileReferenceKey, &fontFileReferenceKeySize)) ||
        FAILED(fontFiles[0]->GetLoader(&fontFileLoader)) ||
        FAILED(fontFileLoader->QueryInterface(__uuidof(IDWriteLocalFontFileLoader), &localFontFileLoader)) ||
        FAILED(localFontFileLoader->GetFilePathLengthFromKey(fontFileReferenceKey, fontFileReferenceKeySize, &filePathLength)))
        return L"";
    std::vector<wchar_t> filePathBuffer(filePathLength + 1);
    if (FAILED(localFontFileLoader->GetFilePathFromKey(fontFileReferenceKey, fontFileReferenceKeySize, filePathBuffer.data(), filePathLength + 1)))
        return L"";
    return std::wstring(filePathBuffer.data(), filePathLength);
}

/**
 * @brief Queues a font for memory loading in a separate thread
 * @param filepath Path to the font file
 * @param fontK Font key identifying the font
 */
void FlarialGUI::queueFontMemoryLoad(std::wstring filepath, FontKey fontK) {
    std::thread([filepath, fontK]() {
        std::ifstream fontFile(filepath, std::ios::binary);
        if (fontFile.is_open()) {
            std::vector<std::byte> fontData = Memory::readFile(filepath);
            fontFile.close();
            FontMemoryToLoad.push_back(std::pair(fontData, fontK));
        }
        else {
            Logger::custom(fg(fmt::color::red), "FontLoadError", "Failed to read font in thread");
        }
        }).detach();
}

/**
 * @brief Loads a font from the system font family
 * @param fontK Font key identifying the font
 * @return True if the font was loaded successfully, false otherwise
 */
bool FlarialGUI::LoadFontFromFontFamily(FontKey fontK) {
    // Load any pending fonts from memory first
    if (!FontMemoryToLoad.empty()) {
        for (auto it = FontMemoryToLoad.begin(); it != FontMemoryToLoad.end(); ) {
            ImFontConfig config;
            config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_MonoHinting;
            config.FontDataOwnedByAtlas = false;
            ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                it->first.data(), static_cast<int>(it->first.size()), it->second.size, &config,
                ImGui::GetIO().Fonts->GetGlyphRangesDefault());
            if (font) {
                FontMap[it->second] = font;
                HasAFontLoaded = true;
                it = FontMemoryToLoad.erase(it);
            }
            else {
                Logger::custom(fg(fmt::color::red), "FontLoadError", "Failed to load font from memory");
                ++it;
            }
        }
    }

    // Prepare font name and path
    std::string name = fontK.name;
    std::transform(name.begin(), name.end(), name.begin(), ::towlower);
    std::wstring fontName = to_wide(name);
    std::wstring fontFilePath = GetFontFilePath(fontName, fontK.weight);
    std::string path;

    // Handle special cases for Minecraft fonts
    if (fontK.name == "162") {
        path = Utils::getAssetsPath() + "\\162" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    }
    else if (fontK.name == "163") {
        path = Utils::getAssetsPath() + "\\163" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    }
    else if (fontK.name == "164") {
        path = Utils::getAssetsPath() + "\\164" + ".ttf";
        fontFilePath = std::wstring(path.begin(), path.end());
    }

    // Check if font file exists
    if (!fontFilePath.empty()) {
        std::ifstream fontFile(fontFilePath, std::ios::binary);
        if (fontFile.is_open()) {
            fontFile.close();
            queueFontMemoryLoad(fontFilePath, fontK);
            return true;
        }
        else {
            Logger::custom(fg(fmt::color::red), "FontLoadError", "Font file not found or inaccessible");
        }
    }

    // If loading fails, mark the font as not found
    FontsNotFound[fontK] = true;
    return false;
}

/**
 * @brief Loads an image from a resource into a bitmap
 * @param resourceId Resource identifier
 * @param bitmap Pointer to store the loaded bitmap
 * @param type Resource type (e.g., "PNG")
 */
void FlarialGUI::LoadImageFromResource(int resourceId, ID2D1Bitmap** bitmap, LPCTSTR type) {
    IWICBitmapDecoder* pDecoder = nullptr;
    IWICBitmapFrameDecode* pFrame = nullptr;
    IWICStream* pStream = nullptr;
    HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    void* pImageFile = LockResource(imageResDataHandle);
    DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    IWICImagingFactory* imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));
    imagingFactory->CreateStream(&pStream);
    pStream->InitializeFromMemory(static_cast<BYTE*>(pImageFile), imageFileSize);
    imagingFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
    pDecoder->GetFrame(0, &pFrame);
    IWICFormatConverter* pConverter = nullptr;
    imagingFactory->CreateFormatConverter(&pConverter);
    pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
    D2D::context->CreateBitmapFromWicBitmap(pConverter, nullptr, bitmap);
    if (pConverter) pConverter->Release();
    if (pDecoder) pDecoder->Release();
    if (pFrame) pFrame->Release();
    if (pStream) pStream->Release();
    imagingFactory->Release();
}

/**
 * @brief Loads an image from a file into a bitmap
 * @param filename Path to the image file
 * @param bitmap Pointer to store the loaded bitmap
 */
void FlarialGUI::LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    IWICBitmapDecoder* bitmapDecoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* formatConverter = nullptr;
    IWICImagingFactory* imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));
    imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);
    bitmapDecoder->GetFrame(0, &frame);
    imagingFactory->CreateFormatConverter(&formatConverter);
    formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
    D2D::context->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);
    bitmapDecoder->Release();
    frame->Release();
    formatConverter->Release();
    imagingFactory->Release();
}

/**
 * @brief Sets up a scroll view clipping region
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param width Width of the scroll view
 * @param height Height of the scroll view
 */
void FlarialGUI::SetScrollView(float x, float y, float width, float height) {
    isInScrollView = true;
    D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
    ScrollViewRect = clipRect;
    PushImClipRect(clipRect);
    D2D::context->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

/**
 * @brief Unsets the scroll view clipping region
 */
void FlarialGUI::UnsetScrollView() {
    isInScrollView = false;
    PopImClipRect();
    D2D::context->PopAxisAlignedClip();
}

/**
 * @brief Sets up a movable window rectangle
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param width Width
 * @param height Height
 * @param currentNum Unique identifier for the window
 * @param fixer Horizontal offset for positioning
 */
void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum, float fixer) {
    isInWindowRect = true;
    D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::White);
    c.a = 0.5f;
    RoundedRect(x, y, c, width, height, 0, 0);
    c.a = 1.f;
    RoundedHollowRect(x, y, Constraints::RelativeConstraint(0.003f, "height", true), c, width, height, 0, 0);
    if (currentNum > maxRect) maxRect = currentNum;
    bool anyMoving = false;
    for (const auto& [key, rect] : WindowRects) {
        if (rect.isMovingElement && key != currentNum) {
            anyMoving = true;
            break;
        }
    }
    WindowRects[currentNum].width = width;
    WindowRects[currentNum].height = height;
    if (!anyMoving) {
        if ((CursorInRect(x, y, width, height) || WindowRects[currentNum].isMovingElement) && MC::held) {
            if (!WindowRects[currentNum].isMovingElement) {
                WindowRects[currentNum].oriMouseX = (MC::mousePos.x - (x + width / 2.0f));
                WindowRects[currentNum].oriMouseY = (MC::mousePos.y - (y + height / 2.0f));
                WindowRects[currentNum].isMovingElement = true;
            }
            WindowRects[currentNum].hasBeenMoved = true;
            WindowRects[currentNum].movedX = ((MC::mousePos.x - WindowRects[currentNum].oriMouseX) - width / 2.0f) + fixer;
            WindowRects[currentNum].movedY = ((MC::mousePos.y - WindowRects[currentNum].oriMouseY) - height / 2.0f);
        }
        if (!MC::held) {
            WindowRects[currentNum].isMovingElement = false;
            WindowRects[currentNum].oriMouseX = -1;
            WindowRects[currentNum].oriMouseY = -1;
        }
    }
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
        for (int j = 0; j <= maxRect; j++) {
            if (j == currentNum) continue;
            auto it = WindowRects.find(j);
            if (it != WindowRects.end()) {
                auto& otherRect = it->second;
                if (fabs(WindowRects[currentNum].movedX - otherRect.movedX - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX, 0), ImVec2(otherRect.movedX, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + fixer;
                }
                if (fabs(WindowRects[currentNum].movedX + width - (otherRect.movedX + otherRect.width) - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width, 0), ImVec2(otherRect.movedX + otherRect.width, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width - width + fixer;
                }
                if (fabs(WindowRects[currentNum].movedY - otherRect.movedY) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY), ImVec2(MC::windowSize.x, otherRect.movedY), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY;
                }
                if (fabs(WindowRects[currentNum].movedY + height - (otherRect.movedY + otherRect.height)) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height - height;
                }
                if (fabs((WindowRects[currentNum].movedX + width / 2.0f) - (otherRect.movedX + otherRect.width / 2.0f) - fixer) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width / 2.0f, 0), ImVec2(otherRect.movedX + otherRect.width / 2.0f, MC::windowSize.y), pink, 2.0f);
                    WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width / 2.0f - width / 2.0f + fixer;
                }
                if (fabs((WindowRects[currentNum].movedY + height / 2.0f) - (otherRect.movedY + otherRect.height / 2.0f)) < alignmentThreshold) {
                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height / 2.0f), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height / 2.0f), pink, 2.0f);
                    WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height / 2.0f - height / 2.0f;
                }
            }
        }
        if (fabs(WindowRects[currentNum].movedX - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX, 0), ImVec2(WindowRects[currentNum].movedX, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = fixer;
        }
        if (fabs(WindowRects[currentNum].movedX + width - MC::windowSize.x - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX + width, 0), ImVec2(WindowRects[currentNum].movedX + width, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = MC::windowSize.x - width + fixer;
        }
        if (fabs(WindowRects[currentNum].movedY) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY), ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY), pink, 2.0f);
            WindowRects[currentNum].movedY = 0.0f;
        }
        if (fabs(WindowRects[currentNum].movedY + height - MC::windowSize.y) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY + height), ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY + height), pink, 2.0f);
            WindowRects[currentNum].movedY = MC::windowSize.y - height;
        }
        if (fabs((WindowRects[currentNum].movedX + width / 2.0f - MC::windowSize.x / 2.0f) - fixer) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(MC::windowSize.x / 2.0f, 0), ImVec2(MC::windowSize.x / 2.0f, MC::windowSize.y), pink, 2.0f);
            WindowRects[currentNum].movedX = MC::windowSize.x / 2.0f - width / 2.0f + fixer;
        }
        if (fabs(WindowRects[currentNum].movedY + height / 2.0f - MC::windowSize.y / 2.0f) < alignmentThreshold) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, MC::windowSize.y / 2.0f), ImVec2(MC::windowSize.x, MC::windowSize.y / 2.0f), pink, 2.0f);
            WindowRects[currentNum].movedY = MC::windowSize.y / 2.0f - height / 2.0f;
        }
    }
}

/**
 * @brief Unsets the window rectangle state
 */
void FlarialGUI::UnsetWindowRect() {
    isInWindowRect = false;
}

/**
 * @brief Calculates the moved coordinates of an element within a window
 * @param x Original X-coordinate
 * @param y Original Y-coordinate
 * @param num Window identifier
 * @param rectWidth Width of the rectangle
 * @param rectHeight Height of the rectangle
 * @return Adjusted coordinates as a Vec2<float>
 */
Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num, float rectWidth, float rectHeight) {
    if (isInWindowRect && WindowRects[num].hasBeenMoved) {
        WindowRects[num].percentageX = WindowRects[num].movedX / MC::windowSize.x;
        WindowRects[num].percentageY = WindowRects[num].movedY / MC::windowSize.y;
        x = WindowRects[num].percentageX * MC::windowSize.x;
        y = WindowRects[num].percentageY * MC::windowSize.y;
    }
    if (x - WindowRects[num].fixer < 0) x = 0.001f - WindowRects[num].fixer;
    if (y < 0) y = 0;
    if (x + rectWidth - WindowRects[num].fixer > MC::windowSize.x) x = MC::windowSize.x - rectWidth + WindowRects[num].fixer;
    if (y + rectHeight > MC::windowSize.y) y = MC::windowSize.y - rectHeight;
    if (x == 0) x = 0.0001f;
    if (y == 0) y = 0.0001f;
    return Vec2<float>{x, y};
}

/**
 * @brief Calculates the center coordinates for a rectangle
 * @param rectWidth Width of the rectangle
 * @param rectHeight Height of the rectangle
 * @return Center coordinates as a Vec2<float>
 */
Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight) {
    Vec2<float> xy;
    xy.x = (D2D::context->GetSize().width - rectWidth) / 2.0f;
    xy.y = (D2D::context->GetSize().height - rectHeight) / 2.0f;
    return xy;
}

/**
 * @brief Resets all GUI element states to their defaults
 */
void FlarialGUI::ResetShit() {
    for (auto& i : WindowRects) i.second = WindowRect();
    for (auto& i : SliderRects) i.second = SliderRect();
    for (auto& i : TextBoxes) i.second = TextBoxStruct();
    for (auto& i : ColorPickers) i.second = ::ColorPicker();
    for (auto& i : DropDownMenus) i.second = DropdownStruct();
    activeColorPickerWindows = 0;
}

static int rotation_start_index;

/**
 * @brief Starts a rotation operation on ImGui draw list
 */
void FlarialGUI::ImRotateStart() {
    rotation_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
}

/**
 * @brief Calculates the center of the current rotation
 * @return Center point as an ImVec2
 */
ImVec2 FlarialGUI::ImRotationCenter() {
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);
    const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

/**
 * @brief Ends a rotation operation on ImGui draw list
 * @param angle Rotation angle in degrees
 * @param center Center point of rotation
 */
void FlarialGUI::ImRotateEnd(float angle, ImVec2 center) {
    float rad = angle * IM_PI / 180.0f;
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;
    auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

/**
 * @brief Pushes an ImGui clip rectangle with position and size
 * @param pos Position of the clip rectangle
 * @param size Size of the clip rectangle
 */
void FlarialGUI::PushImClipRect(ImVec2 pos, ImVec2 size) {
    ImVec2 max(pos.x + size.x, pos.y + size.y);
    ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);
}

/**
 * @brief Pushes an ImGui clip rectangle from a D2D rectangle
 * @param rect D2D rectangle defining the clip area
 */
void FlarialGUI::PushImClipRect(D2D_RECT_F rect) {
    ImVec2 pos(rect.left, rect.top);
    ImVec2 size(rect.right - rect.left, rect.bottom - rect.top);
    ImVec2 max(pos.x + size.x, pos.y + size.y);
    ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);
}

/**
 * @brief Pops the current ImGui clip rectangle
 */
void FlarialGUI::PopImClipRect() {
    ImGui::GetBackgroundDrawList()->PopClipRect();
}

/**
 * @brief Adds a notification to the notification list
 * @param text Text of the notification
 */
void FlarialGUI::Notify(const std::string& text) {
    if (SwapchainHook::init) {
        Notification e;
        e.text = text;
        e.finished = false;
        e.currentPosY = MC::windowSize.y;
        e.arrived = false;
        notifications.push_back(e);
    }
}

/**
 * @brief Updates and renders notifications with animations
 */
void FlarialGUI::NotifyHeartbeat() {
    Vec2<float> round = Constraints::RoundingConstraint(20, 20);
    float height = Constraints::RelativeConstraint(0.035, "height", true);
    float fontSize = Constraints::RelativeConstraint(0.128, "height", true);
    D2D1::ColorF col = colors_primary1;
    int i = 0;
    for (auto it = notifications.begin(); it != notifications.end(); ) {
        Notification& n = *it;
        float posyModif = -((height + Constraints::RelativeConstraint(0.01f, "height", true)) * i);
        if (n.firstTime) {
            std::string sizeName = FlarialTextWithFont(n.currentPos, n.currentPosY, to_wide(n.text).c_str(), 10, 25,
                DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
                D2D1::ColorF(0, 0, 0, 0));
            if (TextSizes[sizeName] != 0) {
                n.width = TextSizes[sizeName] + Constraints::RelativeConstraint(0.0345f, "height", true);
                n.currentPos = Constraints::CenterConstraint(n.width, 0).x;
                n.firstTime = false;
            }
        }
        if (!n.finished) {
            if (!n.arrived) {
                RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
                FlarialTextWithFont(n.currentPos, n.currentPosY + posyModif, to_wide(n.text).c_str(), n.width, height,
                    DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);
                lerp(n.currentPosY, Constraints::PercentageConstraint(0.1f, "bottom", true), frameFactor * 0.067f);
                if (n.currentPosY <= Constraints::PercentageConstraint(0.08f, "bottom", true)) {
                    n.arrived = true;
                    n.time = std::chrono::steady_clock::now();
                }
                ++i;
                ++it;
            }
            else {
                auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - n.time);
                RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
                FlarialTextWithFont(n.currentPos, n.currentPosY + posyModif, to_wide(n.text).c_str(), n.width, height,
                    DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);
                if (timeDifference.count() > 5000) {
                    lerp(n.currentPosY, MC::windowSize.y + 500, frameFactor * 0.052f);
                    if (n.currentPosY >= MC::windowSize.y) {
                        it = notifications.erase(it);
                        continue;
                    }
                }
                ++i;
                ++it;
            }
        }
        else {
            it = notifications.erase(it);
        }
    }
}

/**
 * @brief Copies a bitmap from one to another
 * @param from Source bitmap
 * @param to Destination bitmap pointer
 */
void FlarialGUI::CopyBitmap(ID2D1Bitmap1* from, ID2D1Bitmap** to) {
    if (!from) {
        Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "From is nullptr");
        return;
    }
    if (!*to || from->GetPixelSize() != (*to)->GetPixelSize()) {
        if (*to) (*to)->Release();
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        if (FAILED(D2D::context->CreateBitmap(from->GetPixelSize(), props, to))) {
            Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "Failed to create bitmap");
            return;
        }
    }
    D2D1_POINT_2U destPoint = { 0, 0 };
    D2D1_SIZE_U size = from->GetPixelSize();
    D2D1_RECT_U rect = { 0, 0, size.width, size.height };
    (*to)->CopyFromBitmap(&destPoint, from, &rect);
}

/**
 * @brief Converts a narrow string to a wide string without caching
 * @param str Narrow string to convert
 * @return Converted wide string
 */
std::wstring to_wide_creator(const std::string& str) {
    int wchars_num = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wide(wchars_num, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wide[0], wchars_num);
    return wide;
}

/**
 * @brief Converts a narrow string to a wide string using a cache
 * @param str Narrow string to convert
 * @return Converted wide string
 */
std::wstring FlarialGUI::to_wide(const std::string& str) {
    return toWideCache.getOrInsert(to_wide_creator, str, str);
}

/**
 * @brief Linearly interpolates a value towards a target
 * @param a Current value (modified in place)
 * @param b Target value
 * @param t Interpolation factor (0.0 to 1.0)
 */
template<typename T>
void FlarialGUI::lerp(T& a, const T& b, float t) {
    if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
        float interpolatedValue = a + (b - a) * t;
        a = std::ceilf(interpolatedValue * 1000.0f) / 1000.0f;
    }
    else {
        a = b;
    }
}

/**
 * @brief Enables additional Y offset mode
 */
void FlarialGUI::SetIsInAdditionalYMode() {
    shouldAdditionalY = true;
}

/**
 * @brief Disables additional Y offset mode
 */
void FlarialGUI::UnSetIsInAdditionalYMode() {
    shouldAdditionalY = false;
}

/**
 * @brief Calculates the width of settings text (stub)
 * @param text Text to measure
 * @return Fixed width of 100 (TODO: implement proper calculation)
 */
float FlarialGUI::SettingsTextWidth(const std::string& text) {
    return 100; // TODO: Replace with actual text width calculation
}