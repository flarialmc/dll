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
        if (settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "TAB";

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

                int i3 = 0;
                float i2 = 0;

                for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {

                    i3++;

                    if (i3 % 10 == 1) {
                        i2 += 4.85;
                    }

                }

                Vec2<float> realcenter;

                if (settingperc.x != 0)
                    realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
                        settingperc.y * MC::windowSize.y);
                else
                    realcenter = Constraints::CenterConstraint(i2 * keycardSize, 7.5f * keycardSize, "y", 0.0f, -0.85f);

                i3 = 0;
                i2 = 0;
                int count = 0;

                float fakex = realcenter.x;

                for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {

                    i3++;

                    if (i3 % 10 == 1) {
                        i2 += 4.85;
                        count++;
                        
                        if(count != 1)
                        fakex -= ((5.f * keycardSize) / 2.0f);
                    }

                }

                Vec2<float> rounde = Constraints::RoundingConstraint(
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value,
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value);

                float totalWidth = i2 * keycardSize;

                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)
                    FlarialGUI::SetWindowRect(fakex, realcenter.y, totalWidth, keycardSize * 7.5f, index);

                Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, totalWidth,keycardSize * 7.5f);

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

                FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, i2 * keycardSize, 7.5f * keycardSize,
                    rounde.x,
                    rounde.x);

                int i = 0;

                for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {

                    i++;
                    FlarialGUI::FlarialTextWithFont(fakex + Constraints::SpacingConstraint(0.5, keycardSize), realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize), FlarialGUI::to_wide(Utils::removeNonAlphanumeric(Utils::removeColorCodes(pair.second.name))).c_str(), textColor, keycardSize*5, keycardSize,
                        DWRITE_TEXT_ALIGNMENT_LEADING, fontSize);

                    realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                    if(i % 10 == 0) {
                        realcenter.y = vec2.y;
                        fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                    }

                }


                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

                    FlarialGUI::UnsetWindowRect();
            }
        }
    }
};

