#pragma once
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Utils.hpp"
#include "../D2D.hpp"
#include <wincodec.h>
#include <stack>
#include <map>
#include <vector>
#include <chrono>
#include <unordered_map>

struct HSV {
    float hue;
    float saturation;
    float value;
};

struct WindowRect
{
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    bool madeRect = false;
    float percentageX = 0;
    float percentageY = 0;
    float ratio = 0;
    float oriMouseX = -1;
    float oriMouseY = -1;
    float fixer = 0;
};

struct SliderRect
{
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    float percentageX = 0;
};

struct TextBoxStruct
{
    std::string text;
    bool isActive;
    bool firstTime = true;
    bool isDeleting = false;
    float cursorOpac = 1.0f;
    float cursorX = 0.0f;
    bool isAt1 = false;
};

struct FlarialGradientStops
{
    float position;
    D2D1_COLOR_F color;
};

struct ColorPicker
{
    std::string oldHex;
    float oldOpac;
    std::string newHex;
    float newOpac;
    bool isActive = false;
    float cursorOpac = 1.0f;
    float hueX = 0.0f;
    float oldHueX = 0.0f;
    bool movingHueX = false;
    float opacX = 0.0f;
    bool movingOpacX = false;
    Vec2<float> shade = {-1,-1};
    bool movingShade = false;
    bool isAt1 = false;
};

struct KeybindSelectorer
{
    std::string oldShi;
    std::string newShi;
    bool isActive = false;
    std::chrono::steady_clock::time_point currentOnKeyTime = std::chrono::steady_clock::now();
    D2D1_COLOR_F curColor;
    bool curColorDone = false;
};

struct DropdownStruct
{
    std::string selected;
    std::vector<std::string> options;
    bool isActive = false;
    bool firstTime = true;
    float rotation = 0.0f;
    float yHover = 0.0f;
    int hoveredIndex = 0;
    float opacityHover = 0.0f;
    D2D1_COLOR_F curColor;
    bool curColorDone = false;
    float yChilds = 0.0f;
    bool offsettedQ = false;
    float offsetted = 0.0f;
};

struct ToolTipStruct
{
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
    bool hovering = false;
    float hoverX;
    float hoverY;
    bool in = false;
};

struct ToolTipParams
{
    std::string id;
    float x;
    float y;
    std::string text;
    float width;
    float height;
};

class Dimension
{
public:
    float x = 0;
    float y = 0;
    float height = 0;
    float width = 0;
};

class Notification
{
public:
    float currentPos = 0;
    float currentPosY = 0;
    std::string text;
    bool finished = false;
    bool arrived = false;
    float width = 0;
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
};

class ImagesClass
{
public:
    static std::map<std::string, ID2D1Bitmap*> eimages;
};

namespace FlarialGUI
{
    std::stack<Dimension> inline dimension_stack;
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
    inline WindowRect WindowRects[1000];
    inline SliderRect SliderRects[2500];
    inline TextBoxStruct TextBoxes[1000];
    inline ColorPicker ColorPickers[2000];
    inline DropdownStruct DropDownMenus[2000];
    inline KeybindSelectorer KeybindSelectors[2000];
    inline std::string currentKeybind;
    std::vector<Notification> inline notifications;
    bool inline isInWindowRect = false;

    inline ID2D1Effect* blur = nullptr;
    extern std::unordered_map<std::string, ToolTipStruct> Tooltips;
    extern std::unordered_map<std::string, ID2D1SolidColorBrush*> brushCache;
    extern std::unordered_map<std::string, IDWriteTextFormat*> textFormatCache;
    extern std::unordered_map<std::string, ID2D1GradientStopCollection*> gradientStopCache;
    extern std::unordered_map<std::string, ID2D1LinearGradientBrush*> gradientBrushCache;

    void PushSize(float x, float y, float width, float height);
    void PopSize();
    void PopAllStack();
    bool Button(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width = 100.0f, const float height = 160.0f);
    bool RoundedButton(const int index, const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f, bool glow = false);
    void RoundedRectOnlyTopCorner(const float x, float y, D2D_COLOR_F color, const float width = 160, const float height = 100, const float radiusX = 10, const float radiusY = 10);
    void RoundedRect(const float x, float y, const D2D_COLOR_F color, float width = 160.0f, const float height = 75, const float radiusX = 10.0f, const float radiusY = 10.0f);
    void LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap);
    void RoundedRectWithImageAndText(int index, const float x, float y, const float width, const float height, const D2D1_COLOR_F color, const std::string imagePath, const float imageWidth, const float imageHeight, const wchar_t *text);
    std::wstring to_wide(const std::string &multi);
    void Image(const std::string imageName, const D2D1_RECT_F rect);
    void FlarialText(float x, float y, const wchar_t *text, float width, const float height,
                     const DWRITE_TEXT_ALIGNMENT alignment);
    void SetScrollView(float x, float y, float width, float height);
    void UnsetScrollView();
    Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);
    void ScrollBar(float x, float y, float width, float height, float radius);
    void SetWindowRect(float x, float y, float width, float height, int currentNum, float fixer = 0);
    void UnsetWindowRect();
    Vec2<float> CalculateMovedXY(float x, float y, int num, float rectWidth = 0.0f, float rectHeight = 0.0f);
    Vec2<float> CalculateResizedXY(float x, float y, float width, float height);
    void UpdateWindowRects();

    bool RoundedRadioButton(int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text,
                            const float width, const float height, float radiusX, float radiusY, const std::string& radioName, const std::string& currentRadio);

    void ApplyGaussianBlur(float blurIntensity);

    void ApplyPaintEffect(float blurIntensity);
    void ApplyAestheticDeepFry();
    void ApplyHue(float Hue);
    void ApplyDeepFry(float intensity);
    void ApplyCombinedDeepFry();


    void CopyBitmap(ID2D1Bitmap1 *from, ID2D1Bitmap **to);

    bool Toggle(int index, float x, float y, bool isEnabled);
    bool Toggle(int index, float x, float y, bool isEnabled, bool rgb);

    float Slider(int index, float x, float y, float startingPoint = 50.0f, float maxValue = 100.0f, const float minValue = 0.0f,
                 bool zerosafe = true);

    std::string Dropdown(int index, float x, float y, const std::vector<std::string> options, std::string &value,
                         std::string label);

    void Circle(float x, float y, const D2D1_COLOR_F &color, float radius);

    std::string TextBox(int index, std::string text, int limit, float x, float y, float width, float height);

    void ColorWheel(float x, float y, float radius);

    float HueToRGB(float p, float q, float t);

    HSV RGBtoHSV(D2D1_COLOR_F rgb);

    void HSLToRGB(float h, float s, float l, float &r, float &g, float &b);

    bool CursorInRect(float rectX, float rectY, float width, float height);

    std::string ColorFToHex(const D2D1_COLOR_F &color);

    void
    RoundedHollowRect(float x, float y, float borderWidth, const D2D_COLOR_F color, const float width, const float height, float radiusX,
                      float radiusY);

    template <typename T>
    void lerp(T &a, const T &b, float t);

    D2D_COLOR_F LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float percentage);
    D2D1::ColorF HSVtoColorF(float H, float s, float v);

    void ColorPicker(int index, float x, float y, std::string &hex, float &opacity, bool& rgb);

    void ColorPickerWindow(int index, std::string& hex, float& opacity, bool& rgb);

    D2D1::ColorF HexToColorF(const std::string &hexString);

    void
    FlarialTextWithFont(float x, float y, const wchar_t *text, const float width, const float height,
                        const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
                        const DWRITE_FONT_WEIGHT weight);

    void
    FlarialTextWithFont(float x, float y, const wchar_t *text, const float width, const float height,
                        const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
                        const DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color);

    void ResetShit();

    void AddShadowRect(const D2D1_POINT_2F &obj_min, const D2D1_POINT_2F &obj_max, D2D1_COLOR_F shadow_col,
                       float shadow_thickness, const D2D1_POINT_2F &shadow_offset, float obj_rounding);

    void KeybindSelector(const int index, float x, float y, std::string &keybind);

    void Notify(std::string text);

    void BlurRect(D2D1_ROUNDED_RECT rect, float intensity);

    void ApplyGaussianBlurToTarget(ID2D1BitmapRenderTarget *target, float blurIntensity);

    void NotifyHeartbeat();

    std::string TextBoxVisual(int index, std::string& text, int limit, float x, float y, std::string real = "Text Format");

    ID2D1SolidColorBrush* getBrush(D2D1_COLOR_F color);

    extern IDWriteFactory *writeFactory;

    extern ID2D1ImageBrush* blurbrush;
    extern ID2D1Factory* factory;
    extern ID2D1ImageBrush* shadowbrush;
    extern std::unordered_map<std::string, ID2D1Image*> cachedBitmaps;

    void ShadowRect(D2D1_ROUNDED_RECT rect, D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 0.75f));

    void ApplySusGaussianBlur(float blurIntensity);

    void AllahBlur(float intensity);

    void InnerShadowRect(D2D1_ROUNDED_RECT rect, float howbig, D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 0.75f));

    bool isRectInRect(const D2D1_RECT_F &outer, const D2D1_RECT_F &inner);

    IDWriteTextFormat *getTextFormat(const std::string &fontFamily, FLOAT fontSize, DWRITE_FONT_WEIGHT fontWeight,
                                     DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, DWRITE_TEXT_ALIGNMENT alignment);

    void SetIsInAdditionalYMode();
    void UnSetIsInAdditionalYMode();

    float SettingsTextWidth(std::string text);
    D2D1_COLOR_F getGradientColor(D2D1_GRADIENT_STOP gradientStops[10], float position);
    void RGBController();

    ID2D1LinearGradientBrush *getLinearGradientBrush(float x, float hexPreviewSize, float shadePickerWidth,
                                                     ID2D1GradientStopCollection *pGradientStops, std::string susKey);

    void Tooltip(std::string id, float x, float y, std::string text, float width, float height, bool push = true);
    void displayToolTips();
};