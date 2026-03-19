#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "ToolTipParams.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::unordered_map<std::string, ToolTipParams> tooltipsList;

void FlarialGUI::Tooltip(const std::string& id, float x, float y, const std::string& text, float width, float height, bool push, bool relative, std::chrono::milliseconds duration) {

    if (relative && isInScrollView) y += scrollpos;
    if (push) {
        tooltipsList[id] = ToolTipParams{x, y, text, width, height, relative, "", duration};
        return;
    }

    float fontSize1 = Constraints::RelativeConstraint(0.12, "height", true);
    float spacing = Constraints::RelativeConstraint(0.01f, "height", true);
    float offset = Constraints::RelativeConstraint(0.015, "height", true);

    // Measure tooltip text dimensions (invisible pass — alpha=0)
    tooltips[id].textName = FlarialTextWithFont(spacing + tooltips[id].hoverX + offset, tooltips[id].hoverY - offset,
                    FlarialGUI::to_wide(text).c_str(), 100000.f, 100000.f,
                    DWRITE_TEXT_ALIGNMENT_LEADING, fontSize1, DWRITE_FONT_WEIGHT_REGULAR, D2D1::ColorF(0, 0, 0, 0));

    float rectWidth = TextSizesXY[tooltips[id].textName].x + spacing * 2;
    float rectHeight = TextSizesXY[tooltips[id].textName].y + spacing;

    // Read duration from the push params (tooltipsList entry), falling back to the function parameter
    auto tooltipDuration = tooltipsList.count(id) ? tooltipsList[id].duration : duration;

    // Update hover/opacity state
    if (CursorInRect(x, y, width, height)) {
        if (!tooltips[id].isHovered) {
            tooltips[id].isHovered = true;
            tooltips[id].time = std::chrono::steady_clock::now();
        }
        auto current = std::chrono::steady_clock::now();
        auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - tooltips[id].time);

        if (timeDifference > tooltipDuration) {
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
    } else {
        tooltips[id].hovering = false;
        tooltips[id].isHovered = false;
        lerp(tooltips[id].opac, 0.0f, 0.35f * frameFactor);
    }

    // Snap opacity to exactly 0 when it gets very close (prevents ceilf sticking at ~0.002)
    if (tooltips[id].opac > 0.0f && tooltips[id].opac < 0.005f && !tooltips[id].hovering) {
        tooltips[id].opac = 0.0f;
    }

    // Compute colors AFTER opacity update so they reflect the current frame's value
    if (tooltips[id].opac > 0.01f) {
        D2D1_COLOR_F bgCol = ClickGUI::getColor("secondary2");
        bgCol.a *= tooltips[id].opac;
        D2D1_COLOR_F outlineCol = ClickGUI::getColor("secondary7");
        outlineCol.a *= tooltips[id].opac;
        D2D1_COLOR_F textCol = ClickGUI::getColor("globalText");
        textCol.a *= tooltips[id].opac;

        Vec2<float> round = Constraints::RoundingConstraint(10, 10);

        RoundedRect(MC::mousePos.x + offset, MC::mousePos.y - offset, bgCol, rectWidth, rectHeight, round.x,
                    round.x);
        RoundedHollowRect(MC::mousePos.x + offset, MC::mousePos.y - offset,
                          Constraints::RelativeConstraint(0.001, "height", true), outlineCol, rectWidth, rectHeight,
                          round.x, round.x);
        FlarialTextWithFont(spacing + MC::mousePos.x + offset, MC::mousePos.y - offset,
                            FlarialGUI::to_wide(text).c_str(), rectWidth * 6.9f, rectHeight,
                            DWRITE_TEXT_ALIGNMENT_LEADING, fontSize1, DWRITE_FONT_WEIGHT_REGULAR, textCol);
    }

}

void FlarialGUI::displayToolTips() {

    for (const auto& [id, params] : tooltipsList) {
        if (!id.empty()) {
            Tooltip(id, params.x, params.y, params.text, params.width, params.height, false,
                    params.relative, params.duration);
        }
    }

    tooltipsList.clear();
}
