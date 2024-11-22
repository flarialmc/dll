#include "../../../../Engine.hpp"
#include "../../../../../../Client.hpp"


void RoundedRectBorder(Vec2<float> Position, Vec2<float> Size, ImColor Colour, float Thickness, float Rounding, int Flags) {
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(Position.x, Position.y), ImVec2(Position.x + Size.x, Position.y + Size.y), Colour, Rounding, Flags, Thickness);
}

void FlarialGUI::ShadowRect(Vec2<float> pos, Vec2<float> size, D2D_COLOR_F color, float rounding, int shadowSize) {

    if (Client::settings.getSettingByName<bool>("noshadows")->value) return;

    if (isInScrollView) pos.y += scrollpos;

    ImColor shadowColor(color.r, color.g, color.b, color.a * 0.5f);

    for (int i = 0; i < shadowSize; i++) {
        float progress = static_cast<float>(i) / shadowSize;
        float alphaFactor = (1.0f - progress) * (1.0f - progress);

        ImColor fadedShadowColor = ImColor(
            shadowColor.Value.x,
            shadowColor.Value.y,
            shadowColor.Value.z,
            shadowColor.Value.w * alphaFactor
        );

        Vec2 offset(progress * shadowSize, progress * shadowSize);

        RoundedRectBorder(
            pos - offset,
            size + offset + offset,
            fadedShadowColor,
            2.0f,
            rounding + progress * shadowSize,
            240
        );
    }

    RoundedRectBorder(
        pos,
        size,
        ImColor(color.r, color.g, color.b, color.a),
        1.0f,
        rounding,
        240
    );
}
