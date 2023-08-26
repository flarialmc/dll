#include "Module.hpp"
#include "../../Client.hpp"
#include "ClickGUI/ClickGUIRenderer.hpp"

void Module::NormalRender(int index, std::string text, std::string value) {

    if(settings.getSettingByName<bool>("enabled")->value) {

        std::string uppercaseSentence;
        std::string search = "{VALUE}";

        for (char c: text) {
            uppercaseSentence += std::toupper(c);
        }

        size_t pos = uppercaseSentence.find(search);
        if (pos != std::string::npos) {
            text.replace(pos, search.length(), value);
        }

        float textWidth = Constraints::RelativeConstraint(0.33f * settings.getSettingByName<float>("uiscale")->value);
        float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);

        Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                              this->settings.getSettingByName<float>("percentageY")->value);

        Vec2<float> realcenter;

        if (settingperc.x != 0)
            realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
                                     settingperc.y * MC::windowSize.y);
        else
            realcenter = Constraints::CenterConstraint(textWidth, textHeight);

        float rectWidth = Constraints::RelativeConstraint(0.225f * settings.getSettingByName<float>("uiscale")->value);
        Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value *
                                                             settings.getSettingByName<float>("uiscale")->value,
                                                             this->settings.getSettingByName<float>("rounding")->value *
                                                             settings.getSettingByName<float>("uiscale")->value);

        float realspacing = Constraints::SpacingConstraint(0.155f, textWidth);

        if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value ||
            ClickGUIRenderer::editmenu)
            FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, textHeight, index);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, rectWidth, textHeight);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;

        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        D2D1_COLOR_F bgColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
        D2D1_COLOR_F textColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
        D2D1_COLOR_F borderColor = FlarialGUI::HexToColorF(
                settings.getSettingByName<std::string>("borderColor")->value);

        bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
        textColor.a = settings.getSettingByName<float>("textOpacity")->value;
        borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

        if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
            if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(
                        D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + rectWidth, realcenter.y + textHeight),
                        rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);

        FlarialGUI::RoundedRect(realcenter.x, realcenter.y,
                                bgColor, rectWidth, textHeight,
                                rounde.x, rounde.x);
        FlarialGUI::FlarialTextWithFont(realcenter.x - realspacing, realcenter.y,
                                        FlarialGUI::to_wide(text).c_str(),
                                        textColor, textWidth,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
                                        Constraints::SpacingConstraint(3.2f, textHeight) *
                                        this->settings.getSettingByName<float>("textscale")->value);

        if (this->settings.getSettingByName<bool>("border")->value) {
            FlarialGUI::RoundedHollowRect(realcenter.x, realcenter.y, Constraints::RelativeConstraint(
                                                  (this->settings.getSettingByName<float>("borderWidth")->value *
                                                   (settings.getSettingByName<float>("uiscale")->value) / 100.0f), "height", true),
                                          borderColor, rectWidth, textHeight,
                                          rounde.x, rounde.x);
        }

        if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value ||
            ClickGUIRenderer::editmenu)
            FlarialGUI::UnsetWindowRect();

    }
}