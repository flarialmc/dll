#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "Scripting/Scripting.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/APIUtils.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary3")->value)
#define o_colors_secondary3 clickgui->settings.getSettingByName<float>("o_colors_secondary3")->value
#define colors_secondary3_rgb clickgui->settings.getSettingByName<bool>("colors_secondary3_rgb")->value

#define colors_secondary5 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary5")->value)
#define o_colors_secondary5 clickgui->settings.getSettingByName<float>("o_colors_secondary5")->value
#define colors_secondary5_rgb clickgui->settings.getSettingByName<bool>("colors_secondary5_rgb")->value

#define colors_secondary6 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary6")->value)
#define o_colors_secondary6 clickgui->settings.getSettingByName<float>("o_colors_secondary6")->value
#define colors_secondary6_rgb clickgui->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

#define colors_secondary8 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary8")->value)
#define o_colors_secondary8 clickgui->settings.getSettingByName<float>("o_colors_secondary8")->value
#define colors_secondary8_rgb clickgui->settings.getSettingByName<bool>("colors_secondary8_rgb")->value

class TextPacket;

struct PageType {
    std::string type = "normal";
    std::string module;
};

struct ScrollInfo {
    float scrollpos = 0;
    float barscrollpos = 0;
};

class ClickGUI : public Module {
private:
    float baseHeightReal = 0.f;
    float baseHeightActual = 0.00001f;
    float realBlurAmount = 0.00001f;
    float radioPushAmount1 = 0.0000001f;
    float radioPushAmount2 = 0.0000001f;
    float width1 = 0.000001f;
    float width2 = 0.000001f;
    float width3 = 0.000001f;
    std::string searchBarString;
    Module* ghostMainModule;
public:
    static inline float modcardOpacity = 1.f;
    static inline float settingsOpacity = 0.f;
    static inline float scriptingOpacity = 0.f;
    static inline float modcardOffset = 0.f;
    static inline float settingsOffset = Constraints::RelativeConstraint(2.f, "height", true);
    static inline float scriptingOffset = Constraints::RelativeConstraint(4.f, "height", true);

    static inline PageType page;
    static inline std::string curr = "modules";
    static inline std::map<std::string, ScrollInfo> scrollInfo;
    static float inline accumilatedPos = 1;
    static float inline accumilatedBarPos = 1;
    static bool inline isAnimatingModSet = false;

    bool containsAny(const std::string& str) {
        return std::any_of(APIUtils::onlineUsers.begin(), APIUtils::onlineUsers.end(),
                           [&](const std::string& user) { return str.find(user) != std::string::npos; });
    }

    void onPacketReceive(PacketEvent &event) {
        if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if(pkt->message.length() != 0){
                if (ClickGUI::containsAny(String::removeNonAlphanumeric(String::removeColorCodes(pkt->message)))){ pkt->message = "§f[§4FLARIAL§f]§r " + pkt->message;}
            }
        }
    }

    ClickGUI() : Module("ClickGUI", "What do you think it is?", IDR_CLICKGUI_PNG, "K") {
        Module::setup();
        this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
        scrollInfo["modules"] = { 0, 0 };
        scrollInfo["scripting"] = { 0, 0 };
        scrollInfo["settings"] = { 0, 0 };
    };

    void onSetup() override {
        Listen(this, MouseEvent, &ClickGUI::onMouse)
        Listen(this, KeyEvent, &ClickGUI::onKey)
        if (!Client::settings.getSettingByName<bool>("nochaticon")->value) Listen(this, PacketEvent, &ClickGUI::onPacketReceive)
        ListenOrdered(this, RenderEvent, &ClickGUI::onRender, EventOrder::IMMEDIATE)
        Module::onEnable();
    }

    void onEnable() override {}

    void onDisable() override {}

    void terminate() override {
        Deafen(this, MouseEvent, &ClickGUI::onMouse)
        Deafen(this, KeyEvent, &ClickGUI::onKey)
        Deafen(this, RenderEvent, &ClickGUI::onRender)
        Deafen(this, PacketEvent, &ClickGUI::onPacketReceive)
        Module::terminate();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("editmenubind") == nullptr)
            settings.addSetting("editmenubind", (std::string) "L");

        if (settings.getSettingByName<std::string>("colors_text") == nullptr)
            settings.addSetting("colors_text", (std::string) "ffffff");
        if (settings.getSettingByName<float>("o_colors_text") == nullptr)
            settings.addSetting("o_colors_text", 1.0f);
        if (settings.getSettingByName<bool>("colors_text_rgb") == nullptr)
            settings.addSetting("colors_text_rgb", false);

        if (settings.getSettingByName<std::string>("colors_enabled") == nullptr)
            settings.addSetting("colors_enabled", (std::string) "188830");
        if (settings.getSettingByName<float>("o_colors_enabled") == nullptr)
            settings.addSetting("o_colors_enabled", 1.0f);
        if (settings.getSettingByName<bool>("colors_enabled_rgb") == nullptr)
            settings.addSetting("colors_enabled_rgb", false);

        if (settings.getSettingByName<std::string>("colors_disabled") == nullptr)
            settings.addSetting("colors_disabled", (std::string) "7d1820");
        if (settings.getSettingByName<float>("o_colors_disabled") == nullptr)
            settings.addSetting("o_colors_disabled", 1.0f);
        if (settings.getSettingByName<bool>("colors_disabled_rgb") == nullptr)
            settings.addSetting("colors_disabled_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary1") == nullptr)
            settings.addSetting("colors_primary1", (std::string) "ff233a");
        if (settings.getSettingByName<float>("o_colors_primary1") == nullptr)
            settings.addSetting("o_colors_primary1", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary1_rgb") == nullptr)
            settings.addSetting("colors_primary1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary2") == nullptr)
            settings.addSetting("colors_primary2", (std::string) "ffffff");
        if (settings.getSettingByName<float>("o_colors_primary2") == nullptr)
            settings.addSetting("o_colors_primary2", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary2_rgb") == nullptr)
            settings.addSetting("colors_primary2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary3") == nullptr)
            settings.addSetting("colors_primary3", (std::string) "9a6b72");
        if (settings.getSettingByName<float>("o_colors_primary3") == nullptr)
            settings.addSetting("o_colors_primary3", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary3_rgb") == nullptr)
            settings.addSetting("colors_primary3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary4") == nullptr)
            settings.addSetting("colors_primary4", (std::string) "704b52");
        if (settings.getSettingByName<float>("o_colors_primary4") == nullptr)
            settings.addSetting("o_colors_primary4", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary4_rgb") == nullptr)
            settings.addSetting("colors_primary4_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary1") == nullptr)
            settings.addSetting("colors_secondary1", (std::string) "3f2a2d");
        if (settings.getSettingByName<float>("o_colors_secondary1") == nullptr)
            settings.addSetting("o_colors_secondary1", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary1_rgb") == nullptr)
            settings.addSetting("colors_secondary1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary2") == nullptr)
            settings.addSetting("colors_secondary2", (std::string) "201a1b");
        if (settings.getSettingByName<float>("o_colors_secondary2") == nullptr)
            settings.addSetting("o_colors_secondary2", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary2_rgb") == nullptr)
            settings.addSetting("colors_secondary2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary3") == nullptr)
            settings.addSetting("colors_secondary3", (std::string) "120e0f");
        if (settings.getSettingByName<float>("o_colors_secondary3") == nullptr)
            settings.addSetting("o_colors_secondary3", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary3_rgb") == nullptr)
            settings.addSetting("colors_secondary3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary4") == nullptr)
            settings.addSetting("colors_secondary4", (std::string) "1c1616");
        if (settings.getSettingByName<float>("o_colors_secondary4") == nullptr)
            settings.addSetting("o_colors_secondary4", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary4_rgb") == nullptr)
            settings.addSetting("colors_secondary4_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary5") == nullptr)
            settings.addSetting("colors_secondary5", (std::string) "8b1b25");
        if (settings.getSettingByName<float>("o_colors_secondary5") == nullptr)
            settings.addSetting("o_colors_secondary5", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary5_rgb") == nullptr)
            settings.addSetting("colors_secondary5_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary6") == nullptr)
            settings.addSetting("colors_secondary6", (std::string) "ff2438");
        if (settings.getSettingByName<float>("o_colors_secondary6") == nullptr)
            settings.addSetting("o_colors_secondary6", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary6_rgb") == nullptr)
            settings.addSetting("colors_secondary6_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary7") == nullptr)
            settings.addSetting("colors_secondary7", (std::string) "943c3c");
        if (settings.getSettingByName<float>("o_colors_secondary7") == nullptr)
            settings.addSetting("o_colors_secondary7", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary7_rgb") == nullptr)
            settings.addSetting("colors_secondary7_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary8") == nullptr)
            settings.addSetting("colors_secondary8", (std::string) "302728");
        if (settings.getSettingByName<float>("o_colors_secondary8") == nullptr)
            settings.addSetting("o_colors_secondary8", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary8_rgb") == nullptr)
            settings.addSetting("colors_secondary8_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod1") == nullptr)
            settings.addSetting("colors_mod1", (std::string) "201a1b");
        if (settings.getSettingByName<float>("o_colors_mod1") == nullptr)
            settings.addSetting("o_colors_mod1", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod1_rgb") == nullptr)
            settings.addSetting("colors_mod1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod2") == nullptr)
            settings.addSetting("colors_mod2", (std::string) "2f2022");
        if (settings.getSettingByName<float>("o_colors_mod2") == nullptr)
            settings.addSetting("o_colors_mod2", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod2_rgb") == nullptr)
            settings.addSetting("colors_mod2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod3") == nullptr)
            settings.addSetting("colors_mod3", (std::string) "3f2a2d");
        if (settings.getSettingByName<float>("o_colors_mod3") == nullptr)
            settings.addSetting("o_colors_mod3", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod3_rgb") == nullptr)
            settings.addSetting("colors_mod3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod4") == nullptr)
            settings.addSetting("colors_mod4", (std::string) "705d60");
        if (settings.getSettingByName<float>("o_colors_mod4") == nullptr)
            settings.addSetting("o_colors_mod4", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod4_rgb") == nullptr)
            settings.addSetting("colors_mod4_rgb", false);
    }

    void settingsRender(float settingsOffset) override {
        if (settings.getSettingByName<std::string>("editmenubind")->value.empty())
            settings.getSettingByName<std::string>("editmenubind")->value = "L";

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));


        ModuleManager::getModule("ClickGUI")->addKeybind("Edit Menu Bind", "", settings.getSettingByName<std::string>("editmenubind")->value);

        ModuleManager::getModule("ClickGUI")->addHeader("Colors");
        ModuleManager::getModule("ClickGUI")->addColorPicker("Text Color", "",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_text")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_text")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_text_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Enabled", "",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_enabled")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_enabled")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_enabled_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Disabled", "",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_disabled")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_disabled")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_enabled_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Primary 1", "Color of active elements, main color of sliders , background color of enabled toggles",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_primary1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_primary1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_primary1_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Primary 2", "Minor color of toggles and sliders, text Indicator color",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_primary2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_primary2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_primary2_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Primary 3", "Color of inactive elements, unfilled slider bar color, inner color of color pickers, background color of disabled toggles",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_primary3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_primary3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_primary3_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Primary 4", "Base color of color pickers, hover color of dropdown childrens",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_primary4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_primary4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_primary4_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 1", "Background color of settings",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary1_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 2", "Background color of navigation bar, tooltips, and buttons",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_secondary2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary2_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 3", "Background color of base Click GUI rectangle",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary3_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 4", "Background color of active Modules search bar",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary4_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 5", "Background color of navigation bar icons",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary5")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary5")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary5_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 6", "Background color of active navigation bar buttons",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary6")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary6")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary6_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Secondary 7", "Outline color of tooltips",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_secondary7")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_secondary7")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary7_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Modcard 1", "Color of top rounded rectangle",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_mod1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_mod1")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_mod1_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Modcard 2", "Color of bottom rounded rectangle",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_mod2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_mod2")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_mod2_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Modcard 3", "Background color of module icons, outer color of module settings icon",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_mod3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_mod3")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_mod3_rgb")->value
        );
        ModuleManager::getModule("ClickGUI")->addColorPicker("Modcard 4", "Inner color of module settings icon",
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("colors_mod4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>("o_colors_mod4")->value,
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_mod4_rgb")->value
        );
    }

    static inline bool editmenu = false;

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event);

    void onKey(KeyEvent &event) {

        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()

        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key) && event.getAction() == ActionType::Pressed) {
            if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen")
                this->active = false;
            else {

                if (!Client::settings.getSettingByName<bool>("nochaticon")->value) Listen(this, PacketEvent, &ClickGUI::onPacketReceive)
                else Deafen(this, PacketEvent, &ClickGUI::onPacketReceive);

                keybindActions[0]({});
            }

            if (this->active) {
                accumilatedPos = 0;
                accumilatedBarPos = 0;

                page.type = "normal";
                curr = "modules";
            }
            else {
                FlarialGUI::ResetShit();
                ModuleManager::SaveModulesConfig();
                Client::SaveSettings();
            }
        }

        // if clicked esc
        if ((event.getKey() == VK_ESCAPE && event.getAction() == ActionType::Released)) {
            if (!editmenu) {
                if (this->active) {
                    // exit ClickGUI
                    SDK::clientInstance->grabMouse(); // let mouse control the view

                    this->active = false;
                    page.type = "normal";
                    curr = "modules";

                    FlarialGUI::ResetShit();
                    ModuleManager::SaveModulesConfig();
                    Client::SaveSettings();
                }
            } else {
                // switch from edit mode back to ClickGUI
                editmenu = false;
                this->active = true;
            }
        }

        if (this->active) {
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

            // auto search? TODO: make it optional
            if (page.type == "normal" && curr == "modules" &&
                event.getAction() == ActionType::Pressed) {

                //FlarialGUI::TextBoxes[0].isActive = true;
                // you searching
                if (FlarialGUI::TextBoxes[0].isActive) {
                    FlarialGUI::scrollpos = 0;
                    FlarialGUI::barscrollpos = 0;
                    accumilatedPos = 0;
                    accumilatedBarPos = 0;
                }
            }

            if (this->isAdditionalKeybind(event.keys,
                                          this->settings.getSettingByName<std::string>("editmenubind")->value) &&
                Module::isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>(
                        "editmenubind")->value)) {

                if (!editmenu) {
                    this->active = false;
                    FlarialGUI::Notify("To disable this menu press ESC or " +
                                               this->settings.getSettingByName<std::string>("editmenubind")->value);
                    editmenu = true;
                }

            }
            // switch back to ClickGUI if clicked on Edit Mode bind
        } else if (editmenu && this->isAdditionalKeybind(event.keys, this->settings.getSettingByName<std::string>(
                "editmenubind")->value) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                this->settings.getSettingByName<std::string>(
                                                                                        "editmenubind")->value) ||
                   editmenu && this->isKeybind(event.keys) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                                     this->settings.getSettingByName<std::string>(
                                                                                                             "editmenubind")->value)) {

            editmenu = false;
            this->active = true;

        }

        if (this->active || editmenu && SDK::getCurrentScreen() == "hud_screen")
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (this->active || editmenu)
            event.cancel(); // do not pass key event to the game

        FlarialGUI::inMenu = this->active;
    }

    void onMouse(MouseEvent &event) {
        MC::mousePos.x = event.getMouseX();
        MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;

        if (event.getButton() == MouseButton::Scroll) {
            accumilatedPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::scrollposmodifier
                                                                           : -FlarialGUI::scrollposmodifier;
            accumilatedBarPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::barscrollposmodifier
                                                                              : -FlarialGUI::barscrollposmodifier;
        }

        if (this->active) event.cancel();

        if(SDK::getCurrentScreen() != "hud_screen"){
            if(this->active)
                this->active = false;
        }

        if ((this->active || editmenu) && SDK::getCurrentScreen() == "hud_screen")
            event.cancel(); // TODO: modules dont listen for canceled state!!!

    }

    static bool compareEnabled(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
        return obj1->isEnabled() >
               obj2->isEnabled();
    }

    static bool compareNames(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
        return obj1->name < obj2->name;
    }
};