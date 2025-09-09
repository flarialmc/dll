#pragma once

#include "../../../Utils/Utils.hpp"
#include "../../../Config/Settings.hpp"
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <mutex>

// Forward declarations to avoid heavy includes
struct ID2D1Bitmap;
struct ID2D1Image;
struct ID2D1ImageBrush;
struct IDWriteFactory;
struct ID2D1Factory;
struct ID2D1Effect;

using namespace DirectX;

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
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

namespace FlarialGUI {
	extern std::stack<Dimensions> dimensionStack;
	extern std::vector<float> darkenAmounts;
	extern std::vector<float> glowAlphas;
	extern std::vector<float> opacityAmounts;
	extern std::vector<float> toggleSpacings;
	extern std::vector<float> rotationAngles;
	extern std::vector<D2D1_COLOR_F> toggleColors;
	extern std::vector<D2D1_COLOR_F> buttonColors;
	extern std::unordered_map<int, float> additionalY;

	extern D2D1_COLOR_F rgbColor;
	extern float rgbHue;

	extern float frameFactor;
	extern bool shouldAdditionalY;
	extern int additionalIndex;
	extern int highestAddIndexes;
	extern int activeColorPickerWindows;
	extern int activeSliders;

	extern bool isInScrollView;
	extern D2D1_RECT_F ScrollViewRect;

	extern float scrollpos;
	extern float scrollposmodifier;
	extern float barscrollpos;
	extern float barscrollposmodifier;

	extern std::unordered_map<FontKey, ImFont*> FontMap;
	extern std::unordered_map<FontKey, bool> FontsNotFound;

	extern FontKey LoadFontLater;
	extern bool DoLoadFontLater;
	extern bool HasAFontLoaded;
	extern std::vector<std::pair<std::vector<std::byte>, FontKey>> FontMemoryToLoad;

	extern std::string currentKeybind;

	extern bool isInWindowRect;
	extern bool inMenu;
	extern bool resizing;
	extern bool needsBackBuffer;
	extern bool hasLoadedAll;

	extern ID2D1Effect* blur;
	extern ID2D1Effect* shadow_blur;
	extern ID2D1Bitmap* screen_bitmap_cache;
	extern ID2D1Image* blur_bitmap_cache;
	extern ID2D1ImageBrush* blurbrush;

	extern IDWriteFactory* writeFactory;
	extern ID2D1Factory* factory;
	extern std::unordered_map<std::string, ID2D1Image*> cachedBitmaps;

	extern int maxRect;

	// String cache template functions
	namespace detail {
		template<typename T>
		std::unordered_map<T, std::string>& getCache() {
			static std::unordered_map<T, std::string> cache;
			return cache;
		}

		template<typename T>
		std::mutex& getMutex() {
			static std::mutex m;
			return m;
		}
	}

	template<typename T>
	std::string cached_to_string(const T& value);

	// Core utility functions
	std::string GetWeightedName(std::string name, DWRITE_FONT_WEIGHT weight);
	DWRITE_FONT_WEIGHT GetFontWeightFromString(const std::string& weightStr);
	void PushSize(float x, float y, float width, float height);
	void PopSize();
	void PopAllStack();
	std::wstring GetFontFilePath(const std::wstring& fontName, DWRITE_FONT_WEIGHT weight);
	std::string WideToNarrow(const std::wstring& wideStr);
	std::wstring to_wide(const std::string& str);
	Vec2<float> GetCenterXY(float rectWidth = 160.0f, float rectHeight = 75.0f);
	bool CursorInRect(float rectX, float rectY, float width, float height);
	std::string ColorFToHex(const D2D1_COLOR_F& color);
	D2D1::ColorF HexToColorF(const std::string& hexString);
	float SettingsTextWidth(const std::string& text);
	void SetIsInAdditionalYMode();
	void UnSetIsInAdditionalYMode();

	// Color utilities
	D2D_COLOR_F LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float percentage);
	D2D1::ColorF HSVtoColorF(float H, float s, float v);
	void HSLToRGB(float h, float s, float l, float& r, float& g, float& b);
	float HueToRGB(float p, float q, float t);
}