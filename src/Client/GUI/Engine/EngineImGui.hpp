#pragma once

#include "EngineCore.hpp"
#include "Elements/Structs/Notification.hpp"
#include "Elements/Control/KeybindSelector/KeybindSelector.hpp"
#include "Elements/Control/Dropdown/DropdownStruct.hpp"
#include "Elements/Control/ColorPicker/ColorPicker.hpp"
#include "Elements/Control/TextBox/TextBoxStruct.hpp"
#include "Elements/Control/Slider/SliderRect.hpp"
#include "Elements/Control/SliderInt/SliderIntRect.hpp"
#include "Elements/Windows/WindowRect.hpp"
#include "Elements/Control/Tooltip/ToolTipStruct.hpp"
#include "Elements/Structs/HSV.hpp"
#include <imgui/imgui.h>
#include <thread>
#include <future>

namespace FlarialGUI {
	extern std::unordered_map<int, WindowRect> WindowRects;
	extern std::unordered_map<int, SliderRect> SliderRects;
	extern std::unordered_map<int, SliderIntRect> SliderIntRects;
	extern std::unordered_map<int, TextBoxStruct> TextBoxes;
	extern std::unordered_map<int, ColorPicker> ColorPickers;
	extern std::unordered_map<int, DropdownStruct> DropDownMenus;
	extern std::unordered_map<int, KeybindSelector> KeybindSelectors;
	extern std::unordered_map<int, bool> ToggleIsHovering;
	extern std::unordered_map<int, bool> gears;
	extern std::unordered_map<int, bool> buttonsHovered;
	extern std::unordered_map<int, bool> radioButtonsHovered;
	extern std::unordered_map<std::string, ToolTipStruct> tooltips;
	extern std::unordered_map<std::string, float> TextSizes;
	extern std::unordered_map<std::string, Vec2<float>> TextSizesXY;

	extern std::vector<Notification> notifications;

	// ImGui-specific rotation functions
	void ImRotateStart();
	ImVec2 ImRotationCenter();
	void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());

	// ImGui clipboard utilities
	void PushImClipRect(D2D_RECT_F rect, bool overridePreviousClipping = false);
	void PushImClipRect(ImVec2 pos, ImVec2 size, bool overridePreviousClipping = false);
	void PopImClipRect();

	// ImGui text utilities
	ImVec2 getFlarialTextSize(const wchar_t* text, const float width, const float height,
		const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
		const DWRITE_FONT_WEIGHT weight, bool moduleFont = false, bool troll = false);

	// ImGui conversion utilities
	ImColor D2DColorToImColor(D2D1_COLOR_F color);

	// ImGui alpha override utilities
	void OverrideAlphaValues(float percent);
	void ResetOverrideAlphaValues();

	// GUI rendering functions
	void RoundedRect(float x, float y, D2D_COLOR_F color, float width = 160.0f, float height = 75.0,
		float radiusX = 10.0f, float radiusY = 10.0f, ImDrawFlags flags = ImDrawFlags_RoundCornersAll);

	void RoundedRect(bool imgui, float x, float y, ImColor color, float width = 160.0f, float height = 75.0,
		float radiusX = 10.0f, float radiusY = 10.0f);

	bool RoundedButton(int index, float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor,
		const wchar_t* text, float width = 160.0f, float height = 100.0f,
		float radiusX = 10.0f, float radiusY = 10.0f, bool glow = false);

	void RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width = 160,
		float height = 100, float radiusX = 10, float radiusY = 10);

	bool Button(float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor, const wchar_t* text,
		float width = 100.0f, float height = 160.0f);

	void FlarialText(float x, float y, const wchar_t* text, float width, float height, DWRITE_TEXT_ALIGNMENT alignment);

	void SetScrollView(float x, float y, float width, float height);
	void UnsetScrollView();
	void ScrollBar(float x, float y, float width, float height, float radius);
	void SetWindowRect(float x, float y, float width, float height, int currentNum, std::string modname);
	void UnsetWindowRect();
	Vec2<float> CalculateMovedXY(float x, float y, int num, float rectWidth = 0.0f, float rectHeight = 0.0f);
	Vec2<float> CalculateResizedXY(float x, float y, float width, float height);
	void UpdateWindowRects();

	bool RoundedRadioButton(int index, float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor,
		const wchar_t* text,
		float width, float height, float radiusX, float radiusY,
		const std::string& radioName, const std::string& currentRadio);

	void Circle(float x, float y, const D2D1_COLOR_F& color, float radius);
	void RoundedHollowRect(float x, float y, float borderWidth, D2D_COLOR_F color, float width,
		float height, float radiusX,
		float radiusY);

	// Control elements
	void Toggle(int index, float x, float y, bool isEnabled);
	void Toggle(int index, float x, float y, bool isEnabled, bool rgb, std::string moduleName, std::string settingName);
	bool Toggle(int index, float x, float y, bool isEnabled, bool rgb);

	float Slider(int index, float x, float y, float& value, float maxValue = 100.0f, float minValue = 0.0f, bool zerosafe = true, std::string moduleName = "", std::string settingName = "");
	int SliderInt(int index, float x, float y, int& value, int maxValue = 100, int minValue = 0, std::string moduleName = "", std::string settingName = "");

	inline int TextCursorPosition = 0; //Cursor position from right
	std::string TextBox(int index, std::string& text, int limit, float x, float y, float width, float height, int special = 0, std::string moduleName = "", std::string settingName = "");
	std::string TextBoxVisual(int index, std::string& text, int limit, float x, float y, const std::string& real = "", std::string moduleName = "", std::string settingName = "");

	// Color picker
	void ColorWheel(float x, float y, float radius);
	HSV RGBtoHSV(D2D1_COLOR_F rgb);
	void resetColorPicker(size_t index);
	void ColorPicker(const int index, float x, float y, std::string& hex, bool& rgb, std::string moduleName = "", std::string settingName = "");
	void ColorPicker(const int index, float x, float y, std::string moduleName = "", std::string settingName = "");
	void ColorPickerWindow(int index, std::string& hex, float& opacity, bool& rgb);
	void ColorPickerWindow(int index, std::string moduleName, std::string settingName);
	D2D1_COLOR_F getGradientColor(D2D1_GRADIENT_STOP gradientStops[10], float position);
	void RGBController();

	// Shadow and effects
	void AddShadowRect(const D2D1_POINT_2F& obj_min, const D2D1_POINT_2F& obj_max, D2D1_COLOR_F shadow_col,
		float shadow_thickness, const D2D1_POINT_2F& shadow_offset, float obj_rounding);
	void ShadowRect(Vec2<float> pos, Vec2<float> size, D2D_COLOR_F color, float rounding, int shadowSize);
	void InnerShadowRect(D2D1_ROUNDED_RECT rect, float howbig, D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 0.75f));
	bool isRectInRect(const D2D1_RECT_F& outer, const D2D1_RECT_F& inner);

	// Input and keybind
	void KeybindSelector(int index, float x, float y, std::string& keybind, std::string moduleName = "", std::string settingName = "");

	// Notifications and tooltips
	void Notify(const std::string& text);
	void NotifyHeartbeat();
	void Tooltip(const std::string& id, float x, float y, const std::string& text, float width, float height, bool push = true, bool relative = false,
		std::chrono::milliseconds duration = std::chrono::milliseconds(1000));
	void displayToolTips();

	// Image and rendering utilities
	void image(const std::string& imageName, D2D1_RECT_F rect);
	void image(int resourceId, D2D1_RECT_F rect, LPCTSTR type = "PNG", bool shouldadd = true, ImColor col = IM_COL32_WHITE);
	void RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
		const D2D1_COLOR_F color, const std::string& imagePath,
		const float imageWidth, const float imageHeight);
	void RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
		const D2D1_COLOR_F color, int iconId, const float imageWidth,
		const float imageHeight);

	// Image loading and caching
	void LoadAllImages();
	std::future<void> LoadImagesAsync();
	void CleanupImageResources();
	void LoadAllImageToCache();
	void ExtractImageResource(int resourceId, std::string fileName, LPCTSTR type);
	void LoadFont(int resourceId);
	void queueFontMemoryLoad(std::wstring filepath, FontKey fontK, int ResourceID = 0);
	bool LoadFontFromFontFamily(FontKey fontK);

	// Dropdown
	std::string Dropdown(int index, float x, float y, const std::vector<std::string>& options, std::string& value, const std::string& label, std::string moduleName = "", std::string settingName = "");

	// Text functions
	std::string
		FlarialTextWithFont(float x, float y, const wchar_t* text, float width, float height,
			DWRITE_TEXT_ALIGNMENT alignment, float fontSize,
			DWRITE_FONT_WEIGHT weight, bool moduleFont = false, bool troll = false);

	std::string
		FlarialTextWithFont(float x, float y, const wchar_t* text, float width, float height,
			DWRITE_TEXT_ALIGNMENT alignment, float fontSize,
			DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color, bool moduleFont = false);

	void ResetShit();
}
