#include "MinecraftUIRenderContext.hpp"

void MinecraftUIRenderContext::drawRectangle(Vec4<float> const& rect, MCCColor const& colour, float alpha, int radius) {
	Memory::CallVFunc<12, void, Vec4<float> const&, MCCColor const&, float, int>(this, rect, colour, alpha, radius);
}

void MinecraftUIRenderContext::fillRectangle(Vec4<float> const& rect, MCCColor const& colour, float alpha) {
	Memory::CallVFunc<13, void, Vec4<float> const&, MCCColor const&, float>(this, rect, colour, alpha);
}
void MinecraftUIRenderContext::setClippingRectangle(Vec4<float> const& pos)
{
    return Memory::CallVFunc<20, void, Vec4<float> const&>(this, pos);
};

void MinecraftUIRenderContext::saveCurrentClippingRectangle()
{
    return Memory::CallVFunc<22, void>(this);
};

void MinecraftUIRenderContext::restoreSavedClippingRectangle()
{
    return Memory::CallVFunc<23, void>(this);
};

void MinecraftUIRenderContext::flushString(float delta) {
    return Memory::CallVFunc<6, void, float>(this, delta);
};

__int64 MinecraftUIRenderContext::flushImages(MCCColor& color, float opacity, class StringHasher& hashedString)
{
	return Memory::CallVFunc<9, __int64, MCCColor&, float, class StringHasher&>(this, color, opacity, hashedString);
};

class FontRepos
{
public:
	static inline std::vector<FontBitmap*> fonts = {};

	static const char* FontName(FontBitmap* font)
	{
		if (font->FontPath[0] == 'f')
			return "Mojangles";

		return "Unknown";
	}

	static void PutFont(FontBitmap* font)
	{
		bool fontExists = false;
		for (FontBitmap* fontPtr : fonts)
		{
			if (font == fontPtr)
				fontExists = true;
		}

		if (not fontExists)
		{
			fonts.push_back(font);
		}
	}

	static FontBitmap* GetFont(const char* font)
	{
		for (FontBitmap* fontPtr : fonts)
		{
			if (strcmp(FontName(fontPtr), font) == 0)
				return fontPtr;
		}

		return nullptr;
	}

	static FontBitmap* GetClientFont()
	{
		return GetFont("Minecraftia");
	}
};

__int64 MinecraftUIRenderContext::drawText(FontBitmap* font, const float* pos, TextHolder* text, const float* color, float alpha, unsigned int textAlignment, const TextMeasureData* textMeasureData, const CaretMeasureData* caretMeasureData) {
    if (!font) font = FontRepos::GetClientFont();
    return Memory::CallVFunc<5, __int64, FontBitmap*, const float*, TextHolder*, const float*, float, unsigned int, const TextMeasureData*, const CaretMeasureData*>(this, font, pos, text, color, alpha, textAlignment, textMeasureData, caretMeasureData);
};

float MinecraftUIRenderContext::getMeasureData(void* font, TextHolder* str, float textSize, bool calcWidth) {
	if (!font) font = FontRepos::GetClientFont();

	if (!calcWidth)
		textSize * 10;

	return Memory::CallVFunc<1, float , void*, TextHolder*, float, bool>(this, font, str, textSize, calcWidth);
}

