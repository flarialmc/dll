#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TabListListener.hpp"


class TabList : public Module {

public:


    TabList() : Module("Tab List", "oAnshul==bari", "\\Flarial\\assets\\list.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();
        EventHandler::registerListener(new TabListListener("TabList", this));
        if (settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "TAB";
        if (settings.getSettingByName<bool>("alphaOrder") == nullptr) settings.addSetting<bool>("alphaOrder", true);
    }

    void DefaultConfig() override {

        if(settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if(settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", false);
            settings.addSetting("borderWidth", 1.0f);
        }

        if(settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 32.0f);

        if(settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string)"000000");
            settings.addSetting("textColor", (std::string)"fafafa");
            settings.addSetting("borderColor", (std::string)"000000");
        }

        if(settings.getSettingByName<float>("bgOpacity") == nullptr) {
            settings.addSetting("bgOpacity", 0.55f);
            settings.addSetting("textOpacity", 1.0f);
            settings.addSetting("borderOpacity", 1.0f);
        }

        if(settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }



        if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
            settings.addSetting("BlurEffect", true);
        }

    }

    void onDisable() override {

        EventHandler::unregisterListener("TabList");

        Module::onDisable();

    }

    void SettingsRender() override {
        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f,
            textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White),
            this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if (FlarialGUI::Toggle(0, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;



        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"Border", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));


        percent = FlarialGUI::Slider(5, toggleX + Constraints::SpacingConstraint(0.60, textWidth) + FlarialGUI::SettingsTextWidth("Border "),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value, 4);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Rounding Start */
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(6, toggleX + FlarialGUI::SettingsTextWidth("Rounding "),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White),
            this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, settings.getSettingByName<std::string>("keybind")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
            FlarialGUI::to_wide("Alphabetical Order").c_str(), D2D1::ColorF(D2D1::ColorF::White),
            textWidth * 2.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(2, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f),
            D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White),
            this->settings.getSettingByName<bool>("alphaOrder")->value))
            this->settings.getSettingByName<bool>("alphaOrder")->value = !this->settings.getSettingByName<bool>(
                "alphaOrder")->value;

        /* Rounding End */

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Translucency").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(4, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("BlurEffect")->value)) this->settings.getSettingByName<bool>("BlurEffect")->value = !this->settings.getSettingByName<bool>("BlurEffect")->value;

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Background").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Background "), toggleY - Constraints::SpacingConstraint(0.017, textWidth), settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("Text "), toggleY * 0.99f, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Border").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Border "), toggleY * 0.99f, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);
        FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

    }

    void NormalRender(int index, std::string text, std::string value) override {

        if (SDK::hasInstanced) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                float keycardSize = Constraints::RelativeConstraint(
                    0.05f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);

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
                float fixer = 0;

                for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {

                    i3++;

                    if (i3 % 10 == 1) {
                        i2 += 4.85;
                        count++;

                        if(count != 1) {
                            fakex -= ((5.f * keycardSize) / 2.0f);
                            fixer += ((5.f * keycardSize) / 2.0f);
                        }
                    }

                }

                Vec2<float> rounde = Constraints::RoundingConstraint(
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value,
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value);

                float totalWidth = i2 * keycardSize;

                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)
                    FlarialGUI::SetWindowRect(fakex, realcenter.y, totalWidth, keycardSize * 7.5f, index, fixer);

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
                D2D1_COLOR_F borderColor = FlarialGUI::HexToColorF(
                    settings.getSettingByName<std::string>("borderColor")->value);

                disabledColor.a = settings.getSettingByName<float>("bgOpacity")->value;
                textColor.a = settings.getSettingByName<float>("textOpacity")->value;
                borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;
                if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
                    if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(fakex, realcenter.y, fakex + (i2 * keycardSize), realcenter.y + (7.5*keycardSize)), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value); 
                if (this->settings.getSettingByName<bool>("border")->value) {
                    FlarialGUI::RoundedHollowRect(fakex, realcenter.y, Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * (settings.getSettingByName<float>("uiscale")->value) / 100.0f), "height", true),
                        borderColor, totalWidth, 7.5f * keycardSize,
                        rounde.x, rounde.x);
                }
                FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, 7.5f * keycardSize,
                    rounde.x,
                    rounde.x);

                int i = 0;

                bool yes = settings.getSettingByName<bool>("alphaOrder")->value;

                if(yes) {

                    auto vecmap = copyMapInAlphabeticalOrder(SDK::clientInstance->getLocalPlayer()->getlevel()->playermap);
                    for (const auto &pair: vecmap) {

                        i++;
                        FlarialGUI::FlarialTextWithFont(fakex + Constraints::SpacingConstraint(0.5, keycardSize),
                                                        realcenter.y +
                                                        Constraints::SpacingConstraint(0.12, keycardSize),
                                                        FlarialGUI::to_wide(Utils::removeNonAlphanumeric(
                                                                Utils::removeColorCodes(pair.second.name))).c_str(),
                                                        textColor, keycardSize * 5, keycardSize,
                                                        DWRITE_TEXT_ALIGNMENT_LEADING, fontSize);

                        realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                        if (i % 10 == 0) {
                            realcenter.y = vec2.y;
                            fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                        }

                    }

                } else {
                    for (const auto &pair: SDK::clientInstance->getLocalPlayer()->getlevel()->playermap) {

                        i++;
                        FlarialGUI::FlarialTextWithFont(fakex + Constraints::SpacingConstraint(0.5, keycardSize),
                                                        realcenter.y +
                                                        Constraints::SpacingConstraint(0.12, keycardSize),
                                                        FlarialGUI::to_wide(Utils::removeNonAlphanumeric(
                                                                Utils::removeColorCodes(pair.second.name))).c_str(),
                                                        textColor, keycardSize * 5, keycardSize,
                                                        DWRITE_TEXT_ALIGNMENT_LEADING, fontSize);

                        realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                        if (i % 10 == 0) {
                            realcenter.y = vec2.y;
                            fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                        }

                    }
                }

                if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

                    FlarialGUI::UnsetWindowRect();
            }
        }
    }

    std::vector<std::pair<mcUUID, PlayerListEntry>> copyMapInAlphabeticalOrder(const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap) {
        std::vector<std::pair<mcUUID, PlayerListEntry>> sortedPairs(sourceMap.begin(), sourceMap.end());

        // Sort the vector based on the 'name' field
        std::sort(sortedPairs.begin(), sortedPairs.end(), [](const auto& a, const auto& b) {
            return a.second.name < b.second.name;
        });

        return sortedPairs;
    }
};

