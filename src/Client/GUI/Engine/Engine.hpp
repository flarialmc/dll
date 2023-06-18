#pragma once
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Utils.hpp"
#include "../D2D.hpp"
#include <wincodec.h>
#include <stack>
#include <map>
#include <vector>

struct WindowRect
{
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    bool madeRect = false;
    float percentageX = 0;
    float percentageY = 0;
};

struct SliderRect
{
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    float percentageX = 0;
};

struct TextBox
{
    std::string text;
    bool isActive;
};


class Dimension
{
public:
    float x = 0;
    float y = 0;
    float height = 0;
    float width = 0;
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
    std::vector<float> inline opacityAmounts(10000);
    std::vector<float> inline toggleSpacings(10000);
    float inline frameFactor = 1;

    inline bool isInScrollView = false;
    float inline scrollpos = 0;
    float inline scrollposmodifier = 16.5f;
    float inline barscrollpos = 0;
    float inline barscrollposmodifier = 3.0f;
    inline WindowRect WindowRects[10];
    inline SliderRect SliderRects[10];
    inline TextBox TextBoxes[10];
    bool inline isInWindowRect = false;

    inline ID2D1Effect* blur = nullptr;



    void PushSize(float x, float y, float width, float height);
    void PopSize();
    void PopAllStack();
    bool Button(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width = 100.0f, const float height = 160.0f);
    bool RoundedButton(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f);
    void RoundedRectOnlyTopCorner(const float x, float y, D2D_COLOR_F color, const float width = 160, const float height = 100, const float radiusX = 10, const float radiusY = 10);
    void RoundedRect(const float x, float y, const D2D_COLOR_F color, float width = 160.0f, const float height = 75, const float radiusX = 10.0f, const float radiusY = 10.0f);
    void LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap);
    void RoundedRectWithImageAndText(const float x, float y, const float width, const float height, const D2D1_COLOR_F color, const std::string imagePath, const float imageWidth, const float imageHeight, const wchar_t *text);
    std::wstring to_wide(const std::string &multi);
    void Image(const std::string imageName, const D2D1_RECT_F rect);
    void FlarialText(const float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height);
    void SetScrollView(float x, float y, float width, float height);
    void UnsetScrollView();
    Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);
    void ScrollBar(float x, float y, float width, float height, float radius);
    void SetWindowRect(float x, float y, float width, float height, int currentNum);
    void UnsetWindowRect();
    Vec2<float> CalculateMovedXY(float x, float y, int num);
    Vec2<float> CalculateResizedXY(float x, float y, float width, float height);
    void UpdateWindowRects();

    bool RoundedRadioButton(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text,
                            const float width, const float height, float radiusX, float radiusY, const std::string& radioName, const std::string& currentRadio);

    void ApplyGaussianBlur(float blurIntensity);


    void CopyBitmap(ID2D1Bitmap1 *from, ID2D1Bitmap **to);

    bool Toggle(float x, float y, D2D1_COLOR_F color, D2D1_COLOR_F circleColor, bool isEnabled);

    float Slider(int index, float x, float y, const D2D1_COLOR_F color, const D2D1_COLOR_F disabledColor,
           const D2D1_COLOR_F circleColor);

    void Circle(float x, float y, const D2D1_COLOR_F &color, float radius);

    std::string TextBox(int index, float x, float y, float width, float height);
};