#pragma once

#include "../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../SDK/Client/Render/Tessellator/MaterialPtr.h"
#include "../../SDK/Client/Render/Tessellator/Tessellator.h"

class DrawUtils {
public:
	static void setCtx(MinecraftUIRenderContext* ctx, GuiData* gui);
	static void setGameRenderContext(ScreenContext* ctx);
	static void setColor(float r, float g, float b, float a);
	static Tessellator* getTessellator();
	static ScreenContext* getScreenContext();
	static MaterialPtr* getUIMaterial();

	static void addLine(const Vec2<float>& start, const Vec2<float>& end, float lineWidth, MCCColor color = MCCColor());
	static void addBox(const Vec3<float>& lower, const Vec3<float>& upper, float lineWidth, int mode = 1, MCCColor color = MCCColor());
	static void addEntityBox(Actor* ent, float lineWidth = 0.f, MCCColor color = MCCColor());
};
