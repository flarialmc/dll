
#include "../Utils/Logger.h"
#include "../Utils/RenderUtils.h"
#include "../SDK/Minecraft.h"
#include <wincodec.h>



class WindowRect {
public:
	bool isMovingElement = false;
	bool hasBeenMoved = false;
	float movedX = 0;
	float movedY = 0;
	bool madeRect = false;
};

class FlarialGUI
{
public:
	static const float inline darkenAmount = 0.2f;
	static bool inline isInScrollView = false;
	static float inline scrollpos = 0;
	static float inline scrollposmodifier = 10.5f;
	static float inline barscrollpos = 0;
	static float inline barscrollposmodifier = 3.0f;
	static inline WindowRect WindowRects[10];
	static bool inline isInWindowRect = false;
	static bool Button(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 100.0f, const float height = 160.0f);
	static bool RoundedButton(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f);
	static void RoundedRectOnlyTopCorner(const float x, float y, D2D_COLOR_F color, const float width = 160, const float height = 100);
	static void RoundedRect(const float x, float y, const D2D_COLOR_F color, float width = 160.0f, const float height = 75, const float radiusX = 10.0f, const float radiusY = 10.0f);
	static void LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap);
	static void RoundedRectWithImageAndText(const float x, float y, const float width, const float height, const D2D1_COLOR_F color, const wchar_t* imagePath, const int imageWidth, const int imageHeight, const wchar_t* text);
	static std::wstring to_wide(const std::string &multi);
	static void Image(const std::string imageName, const D2D1_RECT_F rect, const int imageWidth, const int imageHeight);
	static void FlarialText(const float x, float y, const wchar_t* text, D2D1_COLOR_F color, const float width, const float height);
	static void SetScrollView(float x, float y, float width, float height);
	static void UnsetScrollView();
	static Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);
	static void ScrollBar(float x, float y, float width, float height, float radius);
	static void SetWindowRect(float x, float y, float width, float height, int currentNum);
	static void UnsetWindowRect();
	static Vec2<float> CalculateMovedXY(float &x, float &y, int num);
};
