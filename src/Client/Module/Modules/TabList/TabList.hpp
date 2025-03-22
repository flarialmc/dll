 #pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../Manager.hpp"
#include "../../../Client.hpp"
 #include "../Nick/NickModule.hpp"


 class TabList : public Module {
public:
    TabList() : Module("Tab List", "Java-like tab list.\nLists the current online players on the server.",
                       IDR_LIST_PNG, "TAB") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &TabList::onRender)
        Listen(this, KeyEvent, &TabList::onKey)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &TabList::onRender)
        Deafen(this, KeyEvent, &TabList::onKey)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
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

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Main");
        this->addSlider("UI Scale", "The Size of Tablist", this->settings.getSettingByName<float>("uiscale")->value, 1.25f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
   "BlurEffect")->value);
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
        if (SDK::hasInstanced && (active || ClickGUI::editmenu)) {
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
                        SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()
                        );

                    for (const auto &rawName : vecmap) {
                        i++;

                        std::string name = String::removeColorCodes(rawName);
                        if (name.empty()) continue;

                        std::string clearedName = String::removeNonAlphanumeric(name);
                        if (clearedName.empty()) clearedName = name;

                        auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);

                        auto module = ModuleManager::getModule("Nick");
                        if (module && module->isEnabled() &&
                            name == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::original))) {
                            name = module->settings.getSettingByName<std::string>("nick")->value;
                            }

                        float xx = 0;

                        if (it != APIUtils::onlineUsers.end()) {
                            std::map<std::string, int> roleLogos = {
                                {"Dev", IDR_CYAN_LOGO_PNG},
                                {"Staff", IDR_WHITE_LOGO_PNG},
                                {"Gamer", IDR_GAMER_LOGO_PNG},
                                {"Booster", IDR_BOOSTER_LOGO_PNG},
                                {"Default", IDR_RED_LOGO_PNG}
                            };

                            static float p1 = 0.175;
                            static float p2 = 0.196;
                            static float p3 = 0.7;
                            static float p4 = 0.77;
                            float width = Constraints::SpacingConstraint(p2, keycardSize);

                            // Default to the red logo
                            int imageResource = roleLogos["Default"];

                            for (const auto &[role, resource] : roleLogos) {
                                if (APIUtils::hasRole(role, clearedName)) {
                                    imageResource = resource;
                                    break;
                                }
                            }

                            // Render the logo
                            FlarialGUI::image(
                                imageResource,
                                D2D1::RectF(
                                    fakex + Constraints::SpacingConstraint(p1, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                    realcenter.y + width + Constraints::SpacingConstraint(0.17f, keycardSize),
                                    fakex + Constraints::SpacingConstraint(p3, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                    realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize)
                                    )
                                    );

                            xx = Constraints::SpacingConstraint(0.5, keycardSize);
                        }

                        // Render the player's name
                        FlarialGUI::FlarialTextWithFont(
                            fakex + xx + Constraints::SpacingConstraint(0.5, keycardSize),
                            realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize),
                            String::StrToWStr(name).c_str(),
                            keycardSize * 5, keycardSize,
                            DWRITE_TEXT_ALIGNMENT_LEADING, fontSize,
                            DWRITE_FONT_WEIGHT_NORMAL, textColor, true
                            );

                        realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                        if (i % 10 == 0) {
                            realcenter.y = vec2.y;
                            fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                        }
                    }
                }
                else {
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

                        if (module && module->isEnabled() &&
                            name == NickModule::backupOri) {
                            name = module->settings.getSettingByName<std::string>("nick")->value;
                        }

                        FlarialGUI::FlarialTextWithFont(fakex + Constraints::SpacingConstraint(0.5, keycardSize),
                                                        realcenter.y +
                                                        Constraints::SpacingConstraint(0.12, keycardSize),
                                                        FlarialGUI::to_wide(String::removeNonAlphanumeric(
                                                                String::removeColorCodes(name))).c_str(),
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

     void onRender(RenderEvent &event) {
         std::string text;
         this->normalRender(20, text);
     }

     void onKey(KeyEvent &event) {
         if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
            keybindActions[0]({});
         }

         if (!this->isKeybind(event.keys)) this->active = false;

     };

     static std::vector<std::string> copyMapInAlphabeticalOrder(const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap) {
         std::vector<std::string> names;

         for (const auto &pair: sourceMap) {
             names.push_back(pair.second.name);
         }

         // Sort the vector by name in a case-insensitive manner
         std::sort(names.begin(), names.end(), [](const auto &a, const auto &b) {
             // Use references to avoid copying strings
             const auto& nameA = a;
             const auto& nameB = b;

             // Compare names case-insensitively
             return std::lexicographical_compare(nameA.begin(), nameA.end(), nameB.begin(), nameB.end(),
                 [](char c1, char c2) {
                     return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
                 });
         });

         return names;
     }
};

