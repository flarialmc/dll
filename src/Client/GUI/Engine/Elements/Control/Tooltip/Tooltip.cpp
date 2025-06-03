#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "ToolTipParams.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::unordered_map<std::string, ToolTipParams> tooltipsList;

void FlarialGUI::Tooltip(const std::string& id, float x, float y, const std::string& text, float width, float height, bool push, bool relative, std::chrono::milliseconds duration) {

    if (relative && isInScrollView) y += scrollpos;
    if (push) {
        tooltipsList[id] = ToolTipParams{x, y, text, width, height, relative, "", duration};
        return;
    }

    float fontSize1 = Constraints::RelativeConstraint(0.12, "height", true);
    //IDWriteTextFormat* textFormat = FlarialGUI::getTextFormat(Client::settings.getSettingByName<std::string>("fontname")->value, fontSize, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING);
    //textLayout->Release();

    D2D1_COLOR_F bgCol = clickgui->getColor("secondary2", "ClickGUI");
    bgCol.a *= tooltips[id].opac;
    D2D1_COLOR_F outlineCol = clickgui->getColor("secondary7", "ClickGUI");
    outlineCol.a *= tooltips[id].opac;
    D2D1_COLOR_F textCol = clickgui->getColor("globalText", "ClickGUI");
    textCol.a *= tooltips[id].opac;


    float spacing = Constraints::RelativeConstraint(0.01f, "height", true);
    float offset = Constraints::RelativeConstraint(0.015, "height", true);

    tooltips[id].textName = FlarialTextWithFont(spacing + tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                    FlarialGUI::to_wide(text).c_str(),  100000.f, 100000.f,
                    DWRITE_TEXT_ALIGNMENT_LEADING, fontSize1, DWRITE_FONT_WEIGHT_REGULAR, D2D1::ColorF(0, 0, 0, 0));

    float rectWidth = TextSizesXY[tooltips[id].textName].x + spacing * 2;
    float rectHeight = TextSizesXY[tooltips[id].textName].y + spacing;

    if (CursorInRect(x, y, width, height)) {
        if (!tooltips[id].in) {
            tooltips[id].in = true;
            tooltips[id].time = std::chrono::steady_clock::now();
        }
        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - tooltips[id].time);

        if (timeDifference > tooltipsList[id].duration) {
            if (!tooltips[id].hovering) {
                tooltips[id].hovering = true;
                tooltips[id].hoverX = MC::mousePos.x;
                tooltips[id].hoverY = MC::mousePos.y;
            }

            if (tooltips[id].hovering && CursorInRect(x, y, width, height)) {
                tooltips[id].hoverX = MC::mousePos.x;
                tooltips[id].hoverY = MC::mousePos.y;
            }

            lerp(tooltips[id].opac, 1.0f, 0.35f * frameFactor);
        }
    } else if (tooltips[id].hovering && CursorInRect(tooltips[id].hoverX, tooltips[id].hoverY, rectWidth, rectHeight)) {
        tooltips[id].in = true;
        lerp(tooltips[id].opac, 1.0f, 0.35f * frameFactor);
    } else {
        tooltips[id].hovering = false;
        tooltips[id].in = false;
        lerp(tooltips[id].opac, 0.0f, 0.35f * frameFactor);
    }

    if (tooltips[id].opac > 0.01f) { //this used to continue rendering the tooltip at a really low opacity
        Vec2<float> round = Constraints::RoundingConstraint(10, 10);

        RoundedRect(tooltips[id].hoverX + offset, tooltips[id].hoverY - offset, bgCol, rectWidth, rectHeight, round.x,
                    round.x);
        RoundedHollowRect(tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                          Constraints::RelativeConstraint(0.001, "height", true), outlineCol, rectWidth, rectHeight,
                          round.x, round.x);
        FlarialTextWithFont(spacing + tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                            FlarialGUI::to_wide(text).c_str(), rectWidth * 6.9f, rectHeight,
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