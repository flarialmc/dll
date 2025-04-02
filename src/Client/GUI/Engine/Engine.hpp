#pragma once

#include "../../../Utils/Utils.hpp"
#include "../D2D.hpp"
#include <wincodec.h>
#include <stack>
#include <map>
#include <vector>
#include <winrt/base.h>
#include <imgui/imgui.h>
#include <unordered_map>
#include "../../../Utils/Memory/LRUCache.hpp"
#include "Elements/Structs/Notification.hpp"
#include "Elements/Control/KeybindSelector/KeybindSelector.hpp"
#include "Elements/Control/Dropdown/DropdownStruct.hpp"
#include "Elements/Control/ColorPicker/ColorPicker.hpp"
#include "Elements/Control/TextBox/TextBoxStruct.hpp"
#include "Elements/Control/Slider/SliderRect.hpp"
#include "Elements/Windows/WindowRect.hpp"
#include "Elements/Control/Tooltip/ToolTipStruct.hpp"
#include "Elements/Structs/HSV.hpp"

using namespace DirectX;

struct BlurInputBuffer
{
    XMFLOAT2 resolution;
    XMFLOAT2 offset;
    XMFLOAT2 halfpixel;
    XMFLOAT2 _dummy;
};


class Blur
{
public:

    static inline ID3D11PixelShader *pUpsampleShader = nullptr;
    static inline ID3D11PixelShader *pDownsampleShader = nullptr;
    static inline ID3D11VertexShader *pVertexShader = nullptr;
    static inline ID3D11InputLayout *pInputLayout = nullptr;

    static inline ID3D11SamplerState *pSampler = nullptr;
    static inline ID3D11Buffer *pVertexBuffer = nullptr;
    static inline ID3D11Buffer *pConstantBuffer = nullptr;
    static inline BlurInputBuffer constantBuffer;

    // RAII
    static void InitializePipeline();
    //static void Cleanup();

    static void RenderToRTV(ID3D11RenderTargetView *, ID3D11ShaderResourceView *, XMFLOAT2);

    static inline std::vector<ID3D11Texture2D *> framebuffers;

    static inline bool hasDoneFrames = false;

    static void RenderBlur(ID3D11RenderTargetView *, int, float);
};

class BlurDX12
{
public:

    static inline ID3D11PixelShader *pUpsampleShader = nullptr;
    static inline ID3D11PixelShader *pDownsampleShader = nullptr;
    static inline ID3D11VertexShader *pVertexShader = nullptr;
    static inline ID3D11InputLayout *pInputLayout = nullptr;

    static inline ID3D11SamplerState *pSampler = nullptr;
    static inline ID3D11Buffer *pVertexBuffer = nullptr;
    static inline ID3D11Buffer *pConstantBuffer = nullptr;
    static inline BlurInputBuffer constantBuffer;

    // RAII
    static void InitializePipeline();
    //static void Cleanup();

    static void RenderBlur(ID3D12GraphicsCommandList* commandList);
};



class Dimensions {
public:
    float x = 0;
    float y = 0;
    float height = 0;
    float width = 0;
};
struct FontKey {
    std::string name;
    DWRITE_FONT_WEIGHT weight;
    int size;

    bool operator==(const FontKey& other) const {
        return weight == other.weight && size == other.size && name == other.name;
    }
};

namespace std {
    template <>
    struct hash<FontKey> {
        std::size_t operator()(const FontKey& key) const {
            std::size_t h1 = std::hash<std::string>{}(key.name);
            std::size_t h2 = std::hash<int>{}(static_cast<int>(key.weight));
            std::size_t h3 = std::hash<int>{}(key.size);
            // Combine the hash values using a method such as XOR and bit shifting.
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

namespace FlarialGUI {
    std::stack<Dimensions> inline dimensionStack;
    std::vector<float> inline darkenAmounts(10000);
    std::vector<float> inline glowAlphas(10000);
    std::vector<float> inline opacityAmounts(100);
    std::vector<float> inline toggleSpacings(100);
    std::vector<float> inline rotationAngles(100, 0);
    std::vector<D2D1_COLOR_F> inline toggleColors(100, D2D1::ColorF(D2D1::ColorF::White));
    std::vector<D2D1_COLOR_F> inline buttonColors(100, D2D1::ColorF(D2D1::ColorF::Red));
    extern std::unordered_map<int, float> additionalY;

    D2D1_COLOR_F inline rgbColor;
    float inline rgbHue = 0.0f;

    float inline frameFactor = 1;
    bool inline shouldAdditionalY = false;
    int inline additionalIndex = 0;
    int inline highestAddIndexes = 0;
    int inline activeColorPickerWindows = 0;
    int inline activeSliders = 0;

    inline bool isInScrollView = false;
    inline D2D1_RECT_F ScrollViewRect = D2D1::RectF();

    float inline scrollpos = 0;
    float inline scrollposmodifier = 0.f;
    float inline barscrollpos = 0;
    float inline barscrollposmodifier = 10.0f;

    std::string GetWeightedName(std::string name, DWRITE_FONT_WEIGHT weight);
    DWRITE_FONT_WEIGHT GetFontWeightFromString(const std::string& weightStr);

    extern std::unordered_map<int, WindowRect> WindowRects;
    extern std::unordered_map<int, SliderRect> SliderRects;
    extern std::unordered_map<int, TextBoxStruct> TextBoxes;
    extern std::unordered_map<int, ColorPicker> ColorPickers;
    extern std::unordered_map<int, DropdownStruct> DropDownMenus;
    extern std::unordered_map<int, KeybindSelector> KeybindSelectors;

    inline int maxRect = 0;



    inline std::unordered_map<FontKey, ImFont*> FontMap = {};
    inline std::unordered_map<FontKey, bool> FontsNotFound = {};

    FontKey inline LoadFontLater;
    bool inline DoLoadFontLater;
    bool inline HasAFontLoaded = false;
    std::vector<std::pair<std::vector<std::byte>, FontKey>> inline FontMemoryToLoad;
    void queueFontMemoryLoad(std::wstring filepath, FontKey fontK);



    inline std::string currentKeybind;

    std::vector<Notification> inline notifications;

    bool inline isInWindowRect = false;
    bool inline inMenu = false;
    bool inline resizing = false;
    bool inline needsBackBuffer = false;
    bool inline hasLoadedAll = false;

    inline ID2D1Effect* blur = nullptr;
    inline ID2D1Effect* shadow_blur = nullptr;
    inline ID2D1Bitmap* screen_bitmap_cache = nullptr;
    inline ID2D1Image* blur_bitmap_cache = nullptr;
    extern ID2D1ImageBrush*  blurbrush;


    extern std::unordered_map<std::string, ToolTipStruct> tooltips;
    extern std::unordered_map<std::string, float> TextSizes;
    extern std::unordered_map<std::string, Vec2<float>> TextSizesXY;

    extern LRUCache<UINT32, winrt::com_ptr<ID2D1SolidColorBrush>> brushCache;
    extern LRUCache<uint64_t, winrt::com_ptr<IDWriteTextLayout>> textLayoutCache;
    extern LRUCache<UINT32, winrt::com_ptr<IDWriteTextFormat>> textFormatCache;
    extern LRUCache<std::wstring, std::string> fromWideCache;
    extern LRUCache<std::string, std::wstring> toWideCache;
    //extern std::unordered_map<std::string, winrt::com_ptr<ID2D1GradientStopCollection>> gradientStopCache;
    extern LRUCache<uint64_t, winrt::com_ptr<ID2D1LinearGradientBrush>> gradientBrushCache;

    void PushSize(float x, float y, float width, float height);
    std::wstring GetFontFilePath(const std::wstring& fontName, DWRITE_FONT_WEIGHT weight);
    std::string WideToNarrow(const std::wstring& wideStr);

#ifndef CACHED_TO_STRING_HPP
#define CACHED_TO_STRING_HPP

#include <string>
#include <unordered_map>
#include <mutex>

    namespace detail {
        template<typename T>
        inline std::unordered_map<T, std::string>& getCache() {
            static std::unordered_map<T, std::string> cache;
            return cache;
        }

        template<typename T>
        inline std::mutex& getMutex() {
            static std::mutex m;
            return m;
        }
    }

    template<typename T>
    inline std::string cached_to_string(const T& value) {
        auto& cache = detail::getCache<T>();
        {
            std::lock_guard<std::mutex> lock(detail::getMutex<T>());
            auto it = cache.find(value);
            if (it != cache.end()) {
                return it->second;
            }
        }
        std::string result = std::to_string(value);
        {
            std::lock_guard<std::mutex> lock(detail::getMutex<T>());
            cache[value] = result;
        }
        return result;
    }

#endif

    void PopSize();

    void PopAllStack();

    winrt::com_ptr<IDWriteTextFormat>
    GetTextFormat(DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
                  float fontSize,
                  DWRITE_FONT_WEIGHT weight, bool moduleFont);

    winrt::com_ptr<IDWriteTextLayout> GetTextLayout(const wchar_t *text,
                                                    DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_CENTER,
                                                    DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
                                                    float fontSize = 14,
                                                    DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
                                                    float maxWidth = 500, float maxHeight = 500,
                                                    bool moduleFont = false);

    void ImRotateStart();
    ImVec2 ImRotationCenter();
    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());

    void RoundedRect(float x, float y, D2D_COLOR_F color, float width = 160.0f, float height = 75.0,
                     float radiusX = 10.0f, float radiusY = 10.0f, ImDrawFlags flags = ImDrawFlags_RoundCornersAll);

    void RoundedRect(bool imgui, float x, float y, ImColor color, float width = 160.0f, float height = 75.0,
        float radiusX = 10.0f, float radiusY = 10.0f);

    bool RoundedButton(int index, float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor,
                       const wchar_t *text, float width = 160.0f, float height = 100.0f,
                       float radiusX = 10.0f, float radiusY = 10.0f, bool glow = false);

    void RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width = 160,
                                  float height = 100, float radiusX = 10, float radiusY = 10);

    bool Button(float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor, const wchar_t *text,
                float width = 100.0f, float height = 160.0f);

    void LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap);

    void FlarialText(float x, float y, const wchar_t *text, float width, float height,DWRITE_TEXT_ALIGNMENT alignment);

    void SetScrollView(float x, float y, float width, float height);

    void UnsetScrollView();

    Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);

    void ScrollBar(float x, float y, float width, float height, float radius);

    void SetWindowRect(float x, float y, float width, float height, int currentNum, float fixer = 0);

    void UnsetWindowRect();

    Vec2<float> CalculateMovedXY(float x, float y, int num, float rectWidth = 0.0f, float rectHeight = 0.0f);

    Vec2<float> CalculateResizedXY(float x, float y, float width, float height);

    void UpdateWindowRects();

    bool RoundedRadioButton(int index, float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor,
                            const wchar_t *text,
                            float width, float height, float radiusX, float radiusY,
                            const std::string &radioName, const std::string &currentRadio);

    void ApplyGaussianBlur();

    void ApplyPaintEffect(float blurIntensity);

    void ApplyAestheticDeepFry();

    void ApplyHue(float Hue);

    void ApplyDeepFry(float intensity);

    void ApplyCombinedDeepFry();

    void CopyBitmap(ID2D1Bitmap1 *from, ID2D1Bitmap **to);

    bool Toggle(int index, float x, float y, bool isEnabled);

    bool Toggle(int index, float x, float y, bool isEnabled, bool rgb);

    float Slider(int index, float x, float y, float& value, float maxValue = 100.0f, float minValue = 0.0f, bool zerosafe = true);

    void Circle(float x, float y, const D2D1_COLOR_F &color, float radius);

    std::string TextBox(int index, const std::string &text, int limit, float x, float y, float width, float height);

    void ColorWheel(float x, float y, float radius);

    float HueToRGB(float p, float q, float t);

    HSV RGBtoHSV(D2D1_COLOR_F rgb);

    void HSLToRGB(float h, float s, float l, float &r, float &g, float &b);

    bool CursorInRect(float rectX, float rectY, float width, float height);

    std::string ColorFToHex(const D2D1_COLOR_F &color);

    void RoundedHollowRect(float x, float y, float borderWidth, D2D_COLOR_F color, float width,
                      float height, float radiusX,
                      float radiusY);

    template<typename T>
    void lerp(T &a, const T &b, float t);

    D2D_COLOR_F LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float percentage);

    D2D1::ColorF HSVtoColorF(float H, float s, float v);

    void ColorPicker(const int index, float x, float y, std::string &hex, bool &rgb);

    void ColorPickerWindow(int index, std::string &hex, float &opacity, bool &rgb);

    D2D1::ColorF HexToColorF(const std::string &hexString);

    std::wstring to_wide(const std::string &str);

    void PushImClipRect(D2D_RECT_F rect);
    void PushImClipRect(ImVec2 pos, ImVec2 size);
    void PrepareBlur(float intensity);


    void PopImClipRect();

    std::string
    FlarialTextWithFont(float x, float y, const wchar_t *text, float width, float height,
                        DWRITE_TEXT_ALIGNMENT alignment, float fontSize,
                        DWRITE_FONT_WEIGHT weight, bool moduleFont = false, bool troll = false);

    std::string
    FlarialTextWithFont(float x, float y, const wchar_t *text, float width, float height,
                        DWRITE_TEXT_ALIGNMENT alignment, float fontSize,
                        DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color, bool moduleFont = false);

    void ResetShit();

    void AddShadowRect(const D2D1_POINT_2F &obj_min, const D2D1_POINT_2F &obj_max, D2D1_COLOR_F shadow_col,
                       float shadow_thickness, const D2D1_POINT_2F &shadow_offset, float obj_rounding);

    void KeybindSelector(int index, float x, float y, std::string &keybind);

    void Notify(const std::string &text);

    void BlurRect(D2D1_ROUNDED_RECT rect);

    void ApplyGaussianBlurToTarget(ID2D1BitmapRenderTarget *target, float blurIntensity);

    void NotifyHeartbeat();

    std::string TextBoxVisual(int index, std::string& text, int limit, float x, float y, const std::string& real = "");

    winrt::com_ptr<ID2D1SolidColorBrush> getBrush(D2D1_COLOR_F color);

    extern IDWriteFactory *writeFactory;

    extern ID2D1Factory *factory;
    extern std::unordered_map<std::string, ID2D1Image *> cachedBitmaps;

    void ShadowRect(Vec2<float> pos, Vec2<float> size, D2D_COLOR_F color, float rounding, int shadowSize);

    void ApplySusGaussianBlur(float blurIntensity);

    void AllahBlur(float intensity);

    void InnerShadowRect(D2D1_ROUNDED_RECT rect, float howbig, D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 0.75f));

    bool isRectInRect(const D2D1_RECT_F &outer, const D2D1_RECT_F &inner);

    void SetIsInAdditionalYMode();

    void UnSetIsInAdditionalYMode();

    float SettingsTextWidth(const std::string &text);

    D2D1_COLOR_F getGradientColor(D2D1_GRADIENT_STOP gradientStops[10], float position);

    void RGBController();

    winrt::com_ptr<ID2D1LinearGradientBrush> getLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
                                                     ID2D1GradientStopCollection *pGradientStops);

    void Tooltip(const std::string &id, float x, float y, const std::string &text, float width, float height, bool push = true, bool relative = false,
        std::chrono::milliseconds duration = std::chrono::milliseconds(1000));

    void displayToolTips();

    void image(const std::string &imageName, D2D1_RECT_F rect);

    void RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
                                     const D2D1_COLOR_F color, const std::string &imagePath,
                                     const float imageWidth, const float imageHeight);

    void LoadAllImages();

    std::string Dropdown(int index, float x, float y, const std::vector<std::string> &options, std::string &value,
                         const std::string &label);

    void image(int resourceId, D2D1_RECT_F rect, LPCTSTR type = "PNG", bool shouldadd = true, ImColor col = IM_COL32_WHITE);

    void LoadAllImageToCache();

    void LoadImageFromResource(int resourceId, ID2D1Bitmap **bitmap, LPCTSTR type = "PNG");

    bool LoadImageFromResource(int resourceId, ID3D11ShaderResourceView** out_srv, LPCTSTR type);

    bool LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type);

    void ExtractImageResource(int resourceId, std::string fileName, LPCTSTR type);

    void LoadFont(int resourceId);

    bool LoadFontFromFontFamily(FontKey fontK);

    void RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
                                     const D2D1_COLOR_F color, int iconId, const float imageWidth,
                                     const float imageHeight);

    ImColor D2DColorToImColor(D2D1_COLOR_F color);

    void OverrideAlphaValues(float percent);

    void ResetOverrideAlphaValues();
}
