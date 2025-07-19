#pragma once
#include "../../SDK/SDK.hpp"
#include "../../SDK/Client/Level/LevelRender/LevelRender.hpp"
#include "../../SDK/Client/Render/Material/MaterialPtr.hpp"
#include "DrawUtils.hpp"

class MCDrawUtil3D {
private:
    LevelRender* levelRenderer;
    ScreenContext* screenContext;
    mce::MaterialPtr* material;
public:
    MCDrawUtil3D(LevelRender* renderer, ScreenContext* ctx, mce::MaterialPtr* material = nullptr);

    void setMaterial(mce::MaterialPtr* material) { this->material = material; }

    void drawLine(Vec3<float> const& pos1, Vec3<float> const& pos2, D2D_COLOR_F const& color, bool immediate = false);
    void drawQuad(Vec3<float> a, Vec3<float> b, Vec3<float> c, Vec3<float> d, D2D_COLOR_F const& col);
    void fillQuad(Vec3<float> a, Vec3<float> b, Vec3<float> c, Vec3<float> d, D2D_COLOR_F const& color);
    void drawBox(AABB const& box, D2D_COLOR_F const& color);
    void flush();
};