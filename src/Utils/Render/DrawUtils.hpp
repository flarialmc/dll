#pragma once

#include "../../SDK/Client/Render/MinecraftUIRenderContext.hpp"

class DrawUtils {
public:
	static void setCtx(MinecraftUIRenderContext* ctx, GuiData* gui);

	static void addLine(Vec2<float> start, Vec2<float> end, float lineWidth, D2D_COLOR_F color);
	static void addBox(Vec3<float> lower, Vec3<float> upper, float lineWidth, int mode = 1, D2D_COLOR_F color = D2D1::ColorF(D2D1::ColorF::White));
	static void addEntityBox(Player* ent, float lineWidth = 0.f, D2D_COLOR_F color = D2D1::ColorF(D2D1::ColorF::White));
};
