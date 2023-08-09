#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TabListListener.hpp"


class TabList : public Module {

public:


    TabList() : Module("Tab List", "oAnshul==bari", "\\Flarial\\assets\\cloudy.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new TabListListener("TabList", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("TabList");

        Module::onDisable();

    }

    void SettingsRender() override {

    }

    void NormalRender(int index, std::string text, std::string value) override {
        if (SDK::hasInstanced) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                float keycardSize = Constraints::RelativeConstraint(
                    0.05f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);
                float spacing = Constraints::RelativeConstraint(
                    0.0015f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);

                Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                    this->settings.getSettingByName<float>("percentageY")->value);

                Vec2<float> realcenter;

                if (settingperc.x != 0)
                    realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
                        settingperc.y * MC::windowSize.y);
                else
                    realcenter = Constraints::CenterConstraint(keycardSize, keycardSize);

                Vec2<float> rounde = Constraints::RoundingConstraint(
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value,
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value);

                float totalWidth = keycardSize * 3 + spacing * 2;

                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

                    FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, totalWidth, totalWidth, index);

                Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, totalWidth,
                    totalWidth);

                realcenter.x = vec2.x;
                realcenter.y = vec2.y;

                realcenter = realcenter;

                Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

                this->settings.setValue("percentageX", percentages.x);
                this->settings.setValue("percentageY", percentages.y);

                float fontSize = Constraints::SpacingConstraint(3, keycardSize);

                D2D1_COLOR_F disabledColor = FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("bgColor")->value);
                D2D1_COLOR_F textColor = FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("textColor")->value);

                disabledColor.a = settings.getSettingByName<float>("bgOpacity")->value;
                textColor.a = settings.getSettingByName<float>("textOpacity")->value;

                FlarialGUI::RoundedRect(realcenter.x, realcenter.y, disabledColor, 15 * keycardSize, 7.5f * keycardSize,
                    rounde.x,
                    rounde.x);

                for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {
                    FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(pair.second.name).c_str(), textColor, keycardSize*5, keycardSize,
                        DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);
                }


                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

                    FlarialGUI::UnsetWindowRect();
            }
        }
    }
};

