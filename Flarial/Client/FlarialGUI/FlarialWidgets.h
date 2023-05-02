
#include "../Utils/Logger.h"
#include "../Utils/RenderUtils.h"
#include "../SDK/Minecraft.h"

class FlarialGUI
{
public:
	static const float inline darkenAmount = 0.2f;
	static void Button(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 100.0f, const float height = 160.0f);
	static void RoundedButton(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f);
};