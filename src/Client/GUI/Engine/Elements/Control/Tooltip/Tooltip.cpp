#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../Structs/ToolTipParams.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_primary1 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_primary2 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary2")->value)
#define o_colors_primary2 clickgui->settings.getSettingByName<float>("o_colors_primary2")->value
#define colors_primary2_rgb clickgui->settings.getSettingByName<bool>("colors_primary2_rgb")->value

#define colors_primary3 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary3")->value)
#define o_colors_primary3 clickgui->settings.getSettingByName<float>("o_colors_primary3")->value
#define colors_primary3_rgb clickgui->settings.getSettingByName<bool>("colors_primary3_rgb")->value

#define colors_primary4 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary4")->value)
#define o_colors_primary4 clickgui->settings.getSettingByName<float>("o_colors_primary4")->value
#define colors_primary4_rgb clickgui->settings.getSettingByName<bool>("colors_primary4_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

std::unordered_map<std::string, ToolTipParams> tooltipsList;

void FlarialGUI::Tooltip(const std::string& id, float x, float y, const std::string& text, float width, float height, bool push,
                         bool relative) {

    if (relative && isInScrollView) y += scrollpos;
    if (push) {
        tooltipsList[id] = ToolTipParams{x, y, text, width, height, relative};
        return;
    }

    float fontSize1 = Constraints::RelativeConstraint(0.12, "height", true);
    //IDWriteTextFormat* textFormat = FlarialGUI::getTextFormat(Client::settings.getSettingByName<std::string>("fontname")->value, fontSize, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING);

    auto textLayout = FlarialGUI::GetTextLayout(FlarialGUI::to_wide(text).c_str(), DWRITE_TEXT_ALIGNMENT_LEADING,
                                                DWRITE_PARAGRAPH_ALIGNMENT_CENTER, fontSize1,
                                                DWRITE_FONT_WEIGHT_REGULAR,
                                                Constraints::PercentageConstraint(1.0f, "left"),
                                                Constraints::RelativeConstraint(0.029, "height", true));

    DWRITE_TEXT_METRICS textMetrics{};
    textLayout->GetMetrics(&textMetrics);
    //textLayout->Release();

    D2D1_COLOR_F bgCol = colors_secondary2_rgb ? rgbColor : colors_secondary2;
    bgCol.a = o_colors_secondary2 * tooltips[id].opac;
    D2D1_COLOR_F outlineCol = colors_secondary7_rgb ? rgbColor : colors_secondary7;
    outlineCol.a = o_colors_secondary7 * tooltips[id].opac;
    D2D1_COLOR_F textCol = colors_text_rgb ? rgbColor : colors_text;
    textCol.a = o_colors_text * tooltips[id].opac;

    float spacing = Constraints::RelativeConstraint(0.01f, "height", true);
    float rectWidth = textMetrics.width + spacing * 2;
    float rectHeight = textMetrics.height + spacing * 2;

    bool display = false;

    if (CursorInRect(x, y, width, height)) {
        if (!tooltips[id].in) {
            tooltips[id].in = true;
            tooltips[id].time = std::chrono::steady_clock::now();
        }
        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - tooltips[id].time);

        if (timeDifference.count() > 999) {
            if (!tooltips[id].hovering) {
                tooltips[id].hovering = true;
                tooltips[id].hoverX = MC::mousePos.x;
                tooltips[id].hoverY = MC::mousePos.y;
            }

            if (tooltips[id].hovering && CursorInRect(x, y, width, height)) {
                tooltips[id].hoverX = MC::mousePos.x;
                tooltips[id].hoverY = MC::mousePos.y;
            }

            display = true;
            lerp(tooltips[id].opac, 1.0f, 0.35f * frameFactor);
        }
    } else if (tooltips[id].hovering && CursorInRect(tooltips[id].hoverX, tooltips[id].hoverY, rectWidth, rectHeight)) {
        display = true;
        tooltips[id].in = true;
        lerp(tooltips[id].opac, 1.0f, 0.35f * frameFactor);
    } else {
        tooltips[id].hovering = false;
        tooltips[id].in = false;
        lerp(tooltips[id].opac, 0.0f, 0.35f * frameFactor);
    }

    if (tooltips[id].opac > 0.1f) {
        Vec2<float> round = Constraints::RoundingConstraint(10, 10);

        float offset = Constraints::RelativeConstraint(0.015, "height", true);

        RoundedRect(tooltips[id].hoverX + offset, tooltips[id].hoverY - offset, bgCol, rectWidth, rectHeight, round.x,
                    round.x);
        RoundedHollowRect(tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                          Constraints::RelativeConstraint(0.001, "height", true), outlineCol, rectWidth, rectHeight,
                          round.x, round.x);
        FlarialTextWithFont(spacing + tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                            FlarialGUI::to_wide(text).c_str(), textMetrics.width * 6.9, rectHeight,
                            DWRITE_TEXT_ALIGNMENT_LEADING, fontSize1, DWRITE_FONT_WEIGHT_REGULAR, textCol);
    }

}

void FlarialGUI::displayToolTips() {

    for (const std::pair<const std::basic_string<char>, ToolTipParams>& i: tooltipsList) {
        if (!i.first.empty()) {
            Tooltip(i.first, i.second.x, i.second.y, i.second.text, i.second.width, i.second.height, false,
                    i.second.relative);
        }
    }

    tooltipsList.clear();
}