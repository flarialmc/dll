#include "../../../../Engine.hpp"

void RRoundedRectBorder(Vec2<float> Position, Vec2<float> Size, ImColor Colour, float Thickness, float Rounding, int Flags) {
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(Position.x, Position.y), ImVec2(Position.x + Size.x, Position.y + Size.y), Colour, Rounding, Flags, Thickness);
}

void FlarialGUI::InnerShadowRect(D2D1_ROUNDED_RECT rect, float shadowSize, D2D1_COLOR_F color) {
    ImColor shadowColor(color.r, color.g, color.b, color.a * 0.5f);
    Vec2 pos(rect.rect.left, rect.rect.top);
    Vec2 size(rect.rect.right, rect.rect.bottom);
    auto rounding = rect.radiusX;

    for (int i = shadowSize; i > 0; i--) {
        float progress = static_cast<float>(i) / shadowSize;
        float alphaFactor = (1.0f - progress) * (1.0f - progress);

        ImColor fadedShadowColor = ImColor(
            shadowColor.Value.x,
            shadowColor.Value.y,
            shadowColor.Value.z,
            shadowColor.Value.w * alphaFactor
        );

        Vec2 offset(progress * shadowSize, progress * shadowSize);

        RRoundedRectBorder(
            pos + offset,
            size - offset - offset,
            fadedShadowColor,
            2.0f,
            0,
            240
        );
    }

    RRoundedRectBorder(
        pos,
        size,
        ImColor(color.r, color.g, color.b, color.a),
        1.0f,
        0,
        240
    );
}