#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUIRenderer.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_primary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_secondary4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary4")->value)
#define o_colors_secondary4 clickgui->settings.getSettingByName<float>("o_colors_secondary4")->value
#define colors_secondary4_rgb clickgui->settings.getSettingByName<bool>("colors_secondary4_rgb")->value

#define colors_enabled FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_enabled")->value)
#define o_colors_enabled clickgui->settings.getSettingByName<float>("o_colors_enabled")->value
#define colors_enabled_rgb clickgui->settings.getSettingByName<bool>("colors_enabled_rgb")->value

#define colors_disabled FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_disabled")->value)
#define o_colors_disabled clickgui->settings.getSettingByName<float>("o_colors_disabled")->value
#define colors_disabled_rgb clickgui->settings.getSettingByName<bool>("colors_disabled_rgb")->value

#define colors_mod1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod1")->value)
#define o_colors_mod1 clickgui->settings.getSettingByName<float>("o_colors_mod1")->value
#define colors_mod1_rgb clickgui->settings.getSettingByName<bool>("colors_mod1_rgb")->value

#define colors_mod2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod2")->value)
#define o_colors_mod2 clickgui->settings.getSettingByName<float>("o_colors_mod2")->value
#define colors_mod2_rgb clickgui->settings.getSettingByName<bool>("colors_mod2_rgb")->value

#define colors_mod3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod3")->value)
#define o_colors_mod3 clickgui->settings.getSettingByName<float>("o_colors_mod3")->value
#define colors_mod3_rgb clickgui->settings.getSettingByName<bool>("colors_mod3_rgb")->value

#define colors_mod4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod4")->value)
#define o_colors_mod4 clickgui->settings.getSettingByName<float>("o_colors_mod4")->value
#define colors_mod4_rgb clickgui->settings.getSettingByName<bool>("colors_mod4_rgb")->value

std::vector<float> searchBarSizes;
std::vector<float> searchCutOutHeights;
static std::string name = "";

std::string ClickGUIElements::SearchBar(int index, std::string &text, int limit, float x, float y) {

    if (ClickGUIRenderer::page.type == "normal" && ClickGUIRenderer::curr == "modules") {

        D2D1_COLOR_F col;

        Vec2<float> round = Constraints::RoundingConstraint(19, 19);

        if (index > searchBarSizes.size() - 1 || index == 0) {
            float nigga = Constraints::RelativeConstraint(0.42, "height");

            searchBarSizes.emplace_back(nigga);
        }

        if (index > searchCutOutHeights.size() - 1 || index == 0) {
            float nigga = Constraints::RelativeConstraint(0.38, "height");

            searchCutOutHeights.emplace_back(nigga);
        }

        if (FlarialGUI::TextBoxes[index].isActive) {
            if (FlarialGUI::TextBoxes[index].isAt1)
                FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
        } else FlarialGUI::TextBoxes[index].cursorOpac = 0;

        if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
        if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

        if (index <= searchBarSizes.size()) {

            const float textWidth = searchBarSizes[index];
            const float percHeight = Constraints::RelativeConstraint(0.42, "height");

            text = FlarialGUI::TextBox(index, text, limit, x - textWidth, y, textWidth, percHeight);

            if (!text.empty()) FlarialGUI::TextBoxes[index].isActive = true;

            if (FlarialGUI::TextBoxes[index].isActive) {

                FlarialGUI::lerp(searchBarSizes[index], Constraints::RelativeConstraint(2.7f, "height"),
                                 0.12f * FlarialGUI::frameFactor);

                FlarialGUI::lerp(searchCutOutHeights[index], Constraints::RelativeConstraint(0.395f, "height"),
                                 0.12f * FlarialGUI::frameFactor);
            } else {

                FlarialGUI::lerp(searchBarSizes[index], Constraints::RelativeConstraint(0.42, "height"),
                                 0.12f * FlarialGUI::frameFactor);

                FlarialGUI::lerp(searchCutOutHeights[index], -0.5f, 0.12f * FlarialGUI::frameFactor);
            }

            col = colors_primary1_rgb ? FlarialGUI::rgbColor : colors_primary1;
            col.a = o_colors_primary1;

            D2D1_COLOR_F searchbg = colors_secondary4_rgb ? FlarialGUI::rgbColor : colors_secondary4;
            searchbg.a = o_colors_secondary4;


            if(FlarialGUI::TextBoxes[index].isActive) FlarialGUI::RoundedRect(x - textWidth, y, searchbg, textWidth, percHeight, round.x,round.x, ImDrawFlags_RoundCornersBottom);
            else  FlarialGUI::RoundedRect(x - textWidth, y, searchbg, textWidth, percHeight, round.x,round.x, ImDrawFlags_RoundCornersAll);


            ImVec2 pos{x - textWidth, y + searchCutOutHeights[index]};
            ImVec2 size{textWidth, percHeight - searchCutOutHeights[index]};

            FlarialGUI::RoundedRect(pos.x, pos.y, col, size.x, size.y, round.x, round.y);


            /*IDWriteTextLayout* textLayout = FlarialGUI::GetTextLayout(
                FlarialGUI::to_wide(text).c_str(),
                DWRITE_TEXT_ALIGNMENT_LEADING,
                DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                Constraints::SpacingConstraint(0.60f, textWidth),
                DWRITE_FONT_WEIGHT_REGULAR, textWidth, percHeight);
            */

            D2D1_COLOR_F cursorCol = D2D1::ColorF(D2D1::ColorF::White);

            cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

            FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
                             (x - textWidth) + Constraints::RelativeConstraint(0.38, "height") +
                             FlarialGUI::TextSizes[name], 0.420f * FlarialGUI::frameFactor);

            FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX,
                                    y + Constraints::RelativeConstraint(0.2f) / 2.0f, cursorCol,
                                    Constraints::RelativeConstraint(0.025f), percHeight / 1.9f, 0, 0);

            if (searchBarSizes[index] > Constraints::RelativeConstraint(0.45, "height")) {
                name = FlarialGUI::FlarialTextWithFont((x - textWidth) + Constraints::RelativeConstraint(0.38, "height"),
                                                y,
                                                FlarialGUI::to_wide(text).c_str(),
                                                textWidth, percHeight,
                                                DWRITE_TEXT_ALIGNMENT_LEADING,
                                                Constraints::SpacingConstraint(0.60f, textWidth),
                                                DWRITE_FONT_WEIGHT_NORMAL);

            }

            FlarialGUI::image(IDR_SEARCH_PNG,
                              D2D1::RectF((x - textWidth) + Constraints::RelativeConstraint(0.245, "height") / 2.0f,
                                          y + Constraints::RelativeConstraint(0.245, "height") / 2.0f,
                                          ((x - textWidth) +
                                           Constraints::RelativeConstraint(0.18, "height") / 2.0f) +
                                          Constraints::RelativeConstraint(0.22, "height"),
                                          (y + Constraints::RelativeConstraint(0.18, "height") / 2.0f) +
                                          Constraints::RelativeConstraint(0.22, "height")));
            return "";
        }
    }

    return "";
}