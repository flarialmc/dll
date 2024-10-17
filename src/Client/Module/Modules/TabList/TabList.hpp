 #pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TabListListener.hpp"
#include "../Nick/NickListener.hpp"
#include "../../Manager.hpp"
#include "../../../Client.hpp"
#include "../ClickGUI/ClickGUIRenderer.hpp"


class TabList : public Module {

public:
    TabList() : Module("Tab List", "Java-like tab list.\nLists the current online players on the server.",
                       IDR_LIST_PNG, "TAB") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new TabListListener("TabList", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("TabList");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("alphaOrder") == nullptr) settings.addSetting<bool>("alphaOrder", true);

        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if (settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", false);
            settings.addSetting("borderWidth", 1.0f);
        }

        if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 32.0f);

        if (settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string) "000000");
            settings.addSetting("textColor", (std::string) "fafafa");
            settings.addSetting("borderColor", (std::string) "000000");
        }

        if (settings.getSettingByName<float>("bgOpacity") == nullptr) {
            settings.addSetting("bgOpacity", 0.55f);
            settings.addSetting("textOpacity", 1.0f);
            settings.addSetting("borderOpacity", 1.0f);
        }

        if (settings.getSettingByName<bool>("bgRGB") == nullptr) {
            settings.addSetting("bgRGB", false);
            settings.addSetting("textRGB", false);
            settings.addSetting("borderRGB", false);
        }

        if (settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }


        if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
            settings.addSetting("BlurEffect", false);
        }

    }

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Main");
        this->addSlider("UI Scale", "The Size of Tablist", this->settings.getSettingByName<float>("uiscale")->value, 1.25f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>("border")->value, "Border Width", "", this->settings.getSettingByName<float>("borderWidth")->value, 4);
        this->addSlider("Rounding", "", this->settings.getSettingByName<float>("rounding")->value);

        this->extraPadding();

        this->addHeader("Misc");
        this->addToggle("Alphabetical Order", "", this->settings.getSettingByName<bool>("alphaOrder")->value);
        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
        this->addColorPicker("Text", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);
        this->addColorPicker("Border", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void normalRender(int index, std::string& value) override {
        if (SDK::hasInstanced && (active || ClickGUIRenderer::editmenu)) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                float keycardSize = Constraints::RelativeConstraint(
                        0.05f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);

                Vec2<float> settingperc{0, 0};

                int i3 = 0;
                float i2 = 0;

                for (const auto &pair: SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {

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

                for (const auto &pair: SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {

                    i3++;
                    if (i3 % 10 == 1) {
                        i2 += 4.85;
                        count++;
                    }
                }

                Vec2<float> rounde = Constraints::RoundingConstraint(
                        this->settings.getSettingByName<float>("rounding")->value *
                        settings.getSettingByName<float>("uiscale")->value,
                        this->settings.getSettingByName<float>("rounding")->value *
                        settings.getSettingByName<float>("uiscale")->value);

                float totalWidth = i2 * keycardSize;

                Vec2<float> vec2 = realcenter;

                realcenter = realcenter;

                float fontSize = Constraints::SpacingConstraint(3, keycardSize);

                D2D1_COLOR_F disabledColor;
                if (settings.getSettingByName<bool>("bgRGB")->value) disabledColor = FlarialGUI::rgbColor;
                else disabledColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);

                D2D1_COLOR_F textColor;
                if (settings.getSettingByName<bool>("textRGB")->value) textColor = FlarialGUI::rgbColor;
                else textColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);

                D2D1_COLOR_F borderColor;
                if (settings.getSettingByName<bool>("borderRGB")->value) borderColor = FlarialGUI::rgbColor;
                else
                    borderColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("borderColor")->value);

                disabledColor.a = settings.getSettingByName<float>("bgOpacity")->value;
                textColor.a = settings.getSettingByName<float>("textOpacity")->value;
                borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;
                if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
                    if (settings.getSettingByName<bool>("BlurEffect")->value)
                        FlarialGUI::BlurRect(D2D1::RoundedRect(
                                                     D2D1::RectF(fakex, realcenter.y, fakex + (i2 * keycardSize),
                                                                 realcenter.y + (7.5f * keycardSize)), rounde.x, rounde.x));
                if (this->settings.getSettingByName<bool>("border")->value) {
                    FlarialGUI::RoundedHollowRect(fakex, realcenter.y, Constraints::RelativeConstraint(
                                                          (this->settings.getSettingByName<float>("borderWidth")->value *
                                                           (settings.getSettingByName<float>("uiscale")->value) / 100.0f), "height", true),
                                                  borderColor, totalWidth, 7.5f * keycardSize,
                                                  rounde.x, rounde.x);
                }
                FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, 7.5f * keycardSize,
                                        rounde.x,
                                        rounde.x);

                int i = 0;

                bool yes = settings.getSettingByName<bool>("alphaOrder")->value;

                if (yes) {

                    auto vecmap = copyMapInAlphabeticalOrder(
                            SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap());
                    for (const auto &pair: vecmap) {

                        i++;
                        // std::string name = Utils::removeNonAlphanumeric(Utils::removeColorCodes(pair.second.name));
                        std::string name = Utils::removeColorCodes(pair.second.name);
                        if (name.empty()) continue;


                        /*
                        auto it = std::find(ModuleManager::onlineUsers.begin(), ModuleManager::onlineUsers.end(), name);
                        auto it2 = std::find(ModuleManager::onlineDevs.begin(), ModuleManager::onlineDevs.end(), name);
                        auto it3 = std::find(ModuleManager::onlineCommites.begin(), ModuleManager::onlineCommites.end(), name);
                        auto it4 = std::find(ModuleManager::onlinePluses.begin(), ModuleManager::onlinePluses.end(), name);
                        auto it5 = std::find(ModuleManager::onlineStaff.begin(), ModuleManager::onlineStaff.end(), name);
                        */

                        // Check if the string was found


                        auto module = ModuleManager::getModule("Nick");

                        if (module->isEnabled() &&
                            name == Utils::removeNonAlphanumeric(Utils::removeColorCodes(NickListener::original))) {
                            name = module->settings.getSettingByName<std::string>("nick")->value;
                        }

                        float xx = 0;

                        /*

                        if (it != ModuleManager::onlineUsers.end()) {
                            FlarialGUI::image(R"(\Flarial\assets\logo.png)",
                                              D2D1::RectF(fakex + Constraints::SpacingConstraint(0.2, keycardSize),
                                                          realcenter.y +
                                                          Constraints::SpacingConstraint(0.12, keycardSize),
                                                          fakex + Constraints::SpacingConstraint(1.1, keycardSize),
                                                          realcenter.y +
                                                          Constraints::SpacingConstraint(1.22, keycardSize)));

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);

                        }

                        if (it2 != ModuleManager::onlineDevs.end()) {
                            FlarialGUI::image(R"(\Flarial\assets\flarial-dev.png)",
                                D2D1::RectF(fakex + Constraints::SpacingConstraint(0.2, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(0.12, keycardSize),
                                    fakex + Constraints::SpacingConstraint(1.1, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(1.22, keycardSize)));

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);

                        }
                        if (it3 != ModuleManager::onlineCommites.end()) {
                            FlarialGUI::image(R"(\Flarial\assets\flarial-contribiutor.png)",
                                D2D1::RectF(fakex + Constraints::SpacingConstraint(0.2, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(0.12, keycardSize),
                                    fakex + Constraints::SpacingConstraint(1.1, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(1.22, keycardSize)));

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);

                        }
                        if (it4 != ModuleManager::onlinePluses.end()) {
                            FlarialGUI::image(R"(\Flarial\assets\flarial-premium.png)",
                                D2D1::RectF(fakex + Constraints::SpacingConstraint(0.2, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(0.12, keycardSize),
                                    fakex + Constraints::SpacingConstraint(1.1, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(1.22, keycardSize)));

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);

                        }
                        if (it5 != ModuleManager::onlineStaff.end()) {
                            FlarialGUI::image(R"(\Flarial\assets\flarial-staff.png)",
                                D2D1::RectF(fakex + Constraints::SpacingConstraint(0.2, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(0.12, keycardSize),
                                    fakex + Constraints::SpacingConstraint(1.1, keycardSize),
                                    realcenter.y +
                                    Constraints::SpacingConstraint(1.22, keycardSize)));

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);

                        }

                        */

                        FlarialGUI::FlarialTextWithFont(fakex + xx + Constraints::SpacingConstraint(0.5, keycardSize),
                                                        realcenter.y +
                                                        Constraints::SpacingConstraint(0.12, keycardSize),
                                                        Utils::StrToWStr(name).c_str(),
                                                        keycardSize * 5, keycardSize,
                                                        DWRITE_TEXT_ALIGNMENT_LEADING, fontSize,
                                                        DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                        realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                        if (i % 10 == 0) {
                            realcenter.y = vec2.y;
                            fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                        }

                    }

                } else {
                    for (const auto &pair: SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {

                        i++;

                        /*

                        std::string name = pair.second.name;

                        auto it = std::find(ModuleManager::onlineUsers.begin(), ModuleManager::onlineUsers.end(), name);

                        // Check if the string was found
                        if (it != ModuleManager::onlineUsers.end()) {
                            name = "[F] " + name;
                        }

                        auto it2 = std::find(ModuleManager::onlineDevs.begin(), ModuleManager::onlineDevs.end(), name);

                        // Check if the string was found
                        if (it2 != ModuleManager::onlineDevs.end()) {
                            name = "[FD] " + name;
                        }

                        auto it3 = std::find(ModuleManager::onlineCommites.begin(), ModuleManager::onlineCommites.end(), name);

                        // Check if the string was found
                        if (it3 != ModuleManager::onlineCommites.end()) {
                            name = "[FC] " + name;
                        }

                        auto it4 = std::find(ModuleManager::onlinePluses.begin(), ModuleManager::onlinePluses.end(), name);

                        // Check if the string was found
                        if (it4 != ModuleManager::onlinePluses.end()) {
                            name = "[FP] " + name;
                        }
                         */

                        auto module = ModuleManager::getModule("Nick");

                        if (module->isEnabled() &&
                            name == NickListener::backupOri) {
                            name = module->settings.getSettingByName<std::string>("nick")->value;
                        }

                        FlarialGUI::FlarialTextWithFont(fakex + Constraints::SpacingConstraint(0.5, keycardSize),
                                                        realcenter.y +
                                                        Constraints::SpacingConstraint(0.12, keycardSize),
                                                        FlarialGUI::to_wide(Utils::removeNonAlphanumeric(
                                                                Utils::removeColorCodes(name))).c_str(),
                                                        keycardSize * 5, keycardSize,
                                                        DWRITE_TEXT_ALIGNMENT_LEADING, fontSize,
                                                        DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                        realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                        if (i % 10 == 0) {
                            realcenter.y = vec2.y;
                            fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                        }

                    }
                }

                // TODO ModuleManager::getModule("ClickGUI") FIND ALL AND REPLACE!!!
            }
        }
    }

    static std::vector<std::pair<mcUUID, PlayerListEntry>>
    copyMapInAlphabeticalOrder(const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap) {
        std::vector<std::pair<mcUUID, PlayerListEntry>> sortedPairs(sourceMap.begin(), sourceMap.end());

        // Sort the vector based on the 'name' field
        std::sort(sortedPairs.begin(), sortedPairs.end(), [](const auto &a, const auto &b) {
            return a.second.name < b.second.name;
        });

        return sortedPairs;
    }
};

