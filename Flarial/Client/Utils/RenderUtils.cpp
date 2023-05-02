#include "RenderUtils.h"

SwapChainDeviceType RenderUtils::type = SwapChainDeviceType::INVALID;
ID2D1DeviceContext* RenderUtils::D2DC = NULL;

void RenderUtils::drawText(ImDrawList* draw, ImVec2 textPos, std::string text, float fontSize = 1.f, ImColor color = ImColor(255.f, 255.f, 255.f)) {
    if (draw == nullptr)
        draw = ImGui::GetBackgroundDrawList();

    draw->AddText(ImGui::GetFont(), fontSize, textPos, ImColor(color.Value.x / 255.f, color.Value.y / 255.f, color.Value.z / 255.f, color.Value.w), text.c_str());
};

ImVec2 RenderUtils::getTextSize(std::string text, float fontSize) {
    auto font = ImGui::GetFont();
    return font->CalcTextSizeA(fontSize, 1000.f, -1.f, text.c_str());
};

void RenderUtils::fillRect(ImDrawList* draw, ImVec4 rectPos, ImColor color, float round = 0.f) {
    if (draw == nullptr)
        draw = ImGui::GetBackgroundDrawList();

    draw->AddRectFilled(ImVec2(rectPos.x, rectPos.y), ImVec2(rectPos.z, rectPos.w), ImColor(color.Value.x / 255.f, color.Value.y / 255.f, color.Value.z / 255.f, color.Value.w), round);
};

void RenderUtils::drawRect(ImDrawList* draw, ImVec4 rectPos, ImColor color, float round = 0.f, float thickness = 1.f) {
    if (draw == nullptr)
        draw = ImGui::GetBackgroundDrawList();

    draw->AddRect(ImVec2(rectPos.x, rectPos.y), ImVec2(rectPos.z, rectPos.w), ImColor(color.Value.x / 255.f, color.Value.y / 255.f, color.Value.z / 255.f, color.Value.w), round, 0, thickness);
};

bool RenderUtils::drawTabButton(ImDrawList* draw, std::string text) {
    ImVec2 p = ImGui::GetCursorScreenPos();

    bool pressed = false;

    if (ImGui::InvisibleButton(text.c_str(), ImVec2(50, 50))) {
        pressed = true;
    }

    ImVec2 rect = ImGui::GetItemRectSize();

    draw->AddRectFilled(ImVec2(0, 0), ImVec2(50, 50), ImColor(212, 41, 50), 2.f);

    return pressed;
}