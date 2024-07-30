#include "Module.hpp"
#include "../../Client.hpp"
#include "ClickGUI/ClickGUIRenderer.hpp"

std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments = {
        {"Left", DWRITE_TEXT_ALIGNMENT_LEADING},
        {"Center", DWRITE_TEXT_ALIGNMENT_CENTER},
        {"Right", DWRITE_TEXT_ALIGNMENT_TRAILING}
};

static std::string name = "";

void Module::normalRender(int index, std::string &value) {
    if(!isEnabled() || SDK::currentScreen != "hud_screen") return;

    std::string text{};
    if(this->settings.getSettingByName<std::string>("text")!=nullptr)
        text = this->settings.getSettingByName<std::string>("text")->value;

    float rotation = this->settings.getSettingByName<float>("rotation")->value;
    ImVec2 rotationCenter;
    DWRITE_TEXT_ALIGNMENT alignment = alignments[this->settings.getSettingByName<std::string>(
            "textalignment")->value];
    bool responsivewidth = this->settings.getSettingByName<bool>("responsivewidth")->value;
    float paddingX = this->settings.getSettingByName<float>("padx")->value;
    float paddingY = this->settings.getSettingByName<float>("pady")->value;


    if (this->settings.getSettingByName<bool>("reversepaddingx")->value)
        paddingX = -(this->settings.getSettingByName<float>("padx")->value);
    if (this->settings.getSettingByName<bool>("reversepaddingy")->value)
        paddingY = -(this->settings.getSettingByName<float>("pady")->value);

    std::string uppercaseSentence;
    std::string search = "{VALUE}";

    for (char c: text) {
        uppercaseSentence += (char)std::toupper(c);
    }

    size_t pos = uppercaseSentence.find(search);
    if (pos != std::string::npos) {
        text.replace(pos, search.length(), value);
    }

    float textWidth = Constraints::RelativeConstraint(0.7f * settings.getSettingByName<float>("uiscale")->value);
    float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);

    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) *
                     this->settings.getSettingByName<float>("textscale")->value;

    Vec2<float> settingperc = Vec2<float>(
            this->settings.getSettingByName<float>("percentageX")->value,
            this->settings.getSettingByName<float>("percentageY")->value
    );

    float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);
    float rectWidth = (!responsivewidth
                       ? (Constraints::RelativeConstraint(
                    0.225f * settings.getSettingByName<float>("uiscale")->value) *
                          this->settings.getSettingByName<float>("rectwidth")->value)
                       : (FlarialGUI::TextSizes[name] + Constraints::SpacingConstraint(2.0, realspacing)) *
                         this->settings.getSettingByName<float>("rectwidth")->value);

    Vec2<float> realcenter;

    if (settingperc.x != 0)
        realcenter = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
    else
        realcenter = Constraints::CenterConstraint(rectWidth, textHeight * this->settings.getSettingByName<float>(
                "rectheight")->value);

    if (ModuleManager::getModule("ClickGUI")->isEnabled() ||
        ClickGUIRenderer::editmenu) {
        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth,
                                  textHeight * this->settings.getSettingByName<float>("rectheight")->value, index);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, rectWidth, textHeight *
                                                                                                      this->settings.getSettingByName<float>(
                                                                                                              "rectheight")->value);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;

        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);
    }

    Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value *
                                                         settings.getSettingByName<float>("uiscale")->value,
                                                         this->settings.getSettingByName<float>("rounding")->value *
                                                         settings.getSettingByName<float>("uiscale")->value);

    D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
                                                                           : FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("bgColor")->value);
    D2D1_COLOR_F textColor = settings.getSettingByName<bool>("textRGB")->value ? FlarialGUI::rgbColor
                                                                               : FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("textColor")->value);
    D2D1_COLOR_F borderColor = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor
                                                                                   : FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("borderColor")->value);

    bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
    textColor.a = settings.getSettingByName<float>("textOpacity")->value;
    borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

    if (rotation > 0.0f) {

        ImVec2 rotationCenter = ImVec2(
            realcenter.x + rectWidth / 2.0f,
            realcenter.y + textHeight * this->settings.getSettingByName<float>("rectheight")->value / 2.0f);

        FlarialGUI::ImRotateStart();
    }



    if (settings.getSettingByName<bool>("BlurEffect")->value)
        FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y,
                                                           realcenter.x + rectWidth,
                                                           realcenter.y + (textHeight) *
                                                                          this->settings.getSettingByName<float>(
                                                                                  "rectheight")->value), rounde.x,
                                               rounde.x));

    FlarialGUI::RoundedRect(
            realcenter.x,
            realcenter.y,
            bgColor,
            rectWidth,
            textHeight * this->settings.getSettingByName<float>("rectheight")->value,
            rounde.x,
            rounde.x
    );

    std::string rname = FlarialGUI::FlarialTextWithFont(
            realcenter.x + Constraints::SpacingConstraint(paddingX, textWidth),
            realcenter.y + Constraints::SpacingConstraint(paddingY, textWidth),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            textHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            textColor,
            true
    );

    name = rname;

    if (this->settings.getSettingByName<bool>("border")->value) {
        FlarialGUI::RoundedHollowRect(
                realcenter.x,
                realcenter.y,
                Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
                                                 settings.getSettingByName<float>("uiscale")->value) / 100.0f,
                                                "height", true),
                borderColor,
                rectWidth,
                textHeight * this->settings.getSettingByName<float>("rectheight")->value,
                rounde.x,
                rounde.x
        );
    }

    if (rotation > 0.0f) {
        FlarialGUI::ImRotateEnd(rotation, rotationCenter);
    }

    if (ModuleManager::getModule("ClickGUI")->isEnabled() ||
        ClickGUIRenderer::editmenu)
        FlarialGUI::UnsetWindowRect();
}

