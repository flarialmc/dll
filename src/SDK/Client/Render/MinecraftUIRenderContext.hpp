#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "ScreenContext.hpp"
#include "textHolder.hpp"
#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../Level/Level.hpp"
#include "../Container/Inventory.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Client/GUI/D2D.hpp"
#include "TextMeasureData.hpp"
#include "Texture2D.hpp"

class MinecraftUIRenderContext {
public:
	BUILD_ACCESS(this, ClientInstance*, clientInstance, 0x8);
	BUILD_ACCESS(this, ScreenContext*, screenContext, 0x10);
	void drawRectangle(Vec4<float> const& rect, MCCColor const& colour, float alpha, int radius);
	void fillRectangle(Vec4<float> const& rect, MCCColor const& colour, float alpha);
	void setClippingRectangle(Vec4<float> const& pos);
	void saveCurrentClippingRectangle();
	void restoreSavedClippingRectangle();
	void flushString(float delta);
	__int64 flushImages(MCCColor& color, float opacity, class StringHasher& hashedString);
	__int64 drawText(FontBitmap* font, const float* pos, TextHolder* text, const float* color, float alpha, unsigned int textAlignment, const TextMeasureData* textMeasureData, const CaretMeasureData* caretMeasureData);
	float getMeasureData(void* font, TextHolder* str, float textSize, bool calcWidth);
};