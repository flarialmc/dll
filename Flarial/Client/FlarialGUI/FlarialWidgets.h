
#include "../Utils/Logger.h"
#include "../Utils/RenderUtils.h"
#include "../SDK/Minecraft.h"
#include <wincodec.h>

class FlarialGUI
{
public:
	static const float inline darkenAmount = 0.2f;
	static bool inline isInScrollView = false;
	static float inline scrollpos = 0;
	static float inline barscrollpos = 0;
	static float inline barscrollposmodifier = 5.0f;
	static bool Button(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 100.0f, const float height = 160.0f);
	static bool RoundedButton(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f);
	static void ModCard(const float x, float y, const wchar_t* modname, const float width = 160, const float height = 100);
	static void RoundedRectOnlyTopCorner(const float x, float y, D2D_COLOR_F color, const float width = 160, const float height = 100);
	static void RoundedRect(const float x, float y, const D2D_COLOR_F color, float width = 160.0f, const float height = 75, const float radiusX = 10.0f, const float radiusY = 10.0f);
	static void LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap);
	static void RoundedRectWithImageAndText(const float x, float y, const float width, const float height, const D2D1_COLOR_F color, const wchar_t* imagePath, const int imageWidth, const int imageHeight, const wchar_t* text);
	static std::wstring to_wide(const std::string &multi);
	static void DrawImage(const std::string imageName, const D2D1_RECT_F rect, const int imageWidth, const int imageHeight);
	static void DrawFlarialText(const float x, float y, const wchar_t* text, D2D1_COLOR_F color, const float width, const float height);
	static void SetScrollView(float x, float y, float width, float height);
	static void UnsetScrollView();
	static Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);
	static void DrawScrollBar(float x, float y, float width, float height, float radius);
};