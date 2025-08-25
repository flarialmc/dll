#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/APIUtils.hpp"
#include "Utils/WinrtUtils.hpp"
#include <chrono>
#include <Modules/Misc/Input/GUIMouseListener.hpp>

#define clickgui ModuleManager::getModule("ClickGUI")

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
    bool grab = false;
    float baseHeightReal = 0.f;
    float realBlurAmount = 0.00001f;
    float radioPushAmount1 = 0.0000001f;
    float radioPushAmount2 = 0.0000001f;
    float width1 = 0.000001f;
    float width2 = 0.000001f;
    float width3 = 0.000001f;
    std::string searchBarString;
    Module *ghostMainModule;

public:
    static inline float baseHeightActual = 0.00001f;
    static inline float modcardOpacity = 1.f;
    static inline float settingsOpacity = 0.f;
    static inline float scriptingOpacity = 0.f;
    static inline float modcardOffset = 0.f;
    static inline float settingsOffset = Constraints::RelativeConstraint(2.f, "height", true);
    static inline float scriptingOffset = Constraints::RelativeConstraint(4.f, "height", true);

    static inline PageType page;
    static inline std::string curr = "modules";
    static inline std::map<std::string, ScrollInfo> scrollInfo;
    static float inline saved_acumilatedPos = 1;
    static float inline saved_acumilatedBarPos = 1;
    static float inline accumilatedPos = 1;
    static float inline accumilatedBarPos = 1;
    static bool inline isAnimatingModSet = false;
    static std::chrono::time_point<std::chrono::high_resolution_clock> favoriteStart;

    static constexpr uint8_t section1stPart{ 0xC2 }, section2ndPart{ 0xA7 };

    static constexpr auto roleColors = std::to_array<std::pair<std::string_view, std::string_view>>({
            {"Dev", "§b"},
            {"Staff", "§f"},
            {"Gamer", "§u"},
            {"Booster", "§d"},
            {"Supporter", "§5"},
            {"Regular", "§4"}
        });

    static inline D2D_COLOR_F getColor(std::string text) {
        D2D_COLOR_F col = clickgui->settings.getSettingByName<bool>(text + "RGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>(text + "Col")->value);
        col.a = clickgui->settings.getSettingByName<float>(text + "Opacity")->value;
        return col;
    }

private:
    static std::optional<std::pair<std::string_view /*first word match*/, size_t /*text index*/>> findFirstOf(std::string_view text, auto&& words) {
        for (auto&& w : words) {
            if (const auto pos = text.find(w); pos != std::string::npos) {
                return std::pair{ std::string_view{ text.data() + pos, w.length() }, pos };
            }
        }
        return {};
    }

    static size_t sanitizedToRawIndex(std::string_view raw, size_t sanIdx);
    static std::string& getMutableTextForWatermark(TextPacket& pkt);

public:
    void onPacketReceive(PacketEvent& event);

    ClickGUI() : Module("ClickGUI", "What do you think it is?", IDR_CLICKGUI_PNG, "K") {
        this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
        scrollInfo["modules"] = {0, 0};
        scrollInfo["scripting"] = {0, 0};
        scrollInfo["settings"] = {0, 0};

        Listen(this, MouseEvent, &ClickGUI::onMouse)
        //Listen(this, FOVEvent, &ClickGUI::fov)
        Listen(this, KeyEvent, &ClickGUI::onKey)
        ListenOrdered(this, PacketEvent, &ClickGUI::onPacketReceive, EventOrder::IMMEDIATE)
        ListenOrdered(this, RenderEvent, &ClickGUI::onRender, EventOrder::IMMEDIATE)
        Listen(this, SetupAndRenderEvent, &ClickGUI::onSetupAndRender)
        //Module::onEnable();
    };

    void onSetup() override {
    }

    void onEnable() override {
    }

    void onDisable() override {
    }

    void terminate() override {
        Deafen(this, MouseEvent, &ClickGUI::onMouse)
        Deafen(this, KeyEvent, &ClickGUI::onKey)
        Deafen(this, RenderEvent, &ClickGUI::onRender)
        Deafen(this, PacketEvent, &ClickGUI::onPacketReceive)
        Module::terminate();
        //
    }

    void defaultConfig() override {
        settings.renameSetting("colors_FlarialLogo", "o_colors_FlarialLogo", "colors_FlarialLogo_rgb", "flariallogo");
        settings.renameSetting("colors_radiobutton_enabled", "o_colors_radiobutton_enabled", "colors_radiobutton_enabled_rgb", "enabledRadioButton");
        settings.renameSetting("colors_radiobutton_disabled", "o_colors_radiobutton_disabled", "colors_radiobutton_disabled_rgb", "disabledRadioButton");
        settings.renameSetting("colors_text", "o_colors_text", "colors_text_rgb", "globalText");
        settings.renameSetting("colors_enabled", "o_colors_enabled", "colors_enabled_rgb", "modCardEnabled");
        settings.renameSetting("colors_disabled", "o_colors_disabled", "colors_disabled_rgb", "modCardDisabled");
        settings.renameSetting("colors_primary1", "o_colors_primary1", "colors_primary1_rgb", "primary1");
        settings.renameSetting("colors_primary2", "o_colors_primary2", "colors_primary2_rgb", "primary2");
        settings.renameSetting("colors_primary3", "o_colors_primary3", "colors_primary3_rgb", "primary3");
        settings.renameSetting("colors_primary4", "o_colors_primary4", "colors_primary4_rgb", "primary4");
        settings.renameSetting("colors_secondary1", "o_colors_secondary1", "colors_secondary1_rgb", "secondary1");
        settings.renameSetting("colors_secondary2", "o_colors_secondary2", "colors_secondary2_rgb", "secondary2");
        settings.renameSetting("colors_secondary3", "o_colors_secondary3", "colors_secondary3_rgb", "secondary3");
        settings.renameSetting("colors_secondary4", "o_colors_secondary4", "colors_secondary4_rgb", "secondary4");
        settings.renameSetting("colors_secondary5", "o_colors_secondary5", "colors_secondary5_rgb", "secondary5");
        settings.renameSetting("colors_secondary6", "o_colors_secondary6", "colors_secondary6_rgb", "secondary6");
        settings.renameSetting("colors_secondary7", "o_colors_secondary7", "colors_secondary7_rgb", "secondary7");
        settings.renameSetting("colors_secondary8", "o_colors_secondary8", "colors_secondary8_rgb", "secondary8");
        settings.renameSetting("colors_mod1", "o_colors_mod1", "colors_mod1_rgb", "modcard1");
        settings.renameSetting("colors_mod2", "o_colors_mod2", "colors_mod2_rgb", "modcard2");
        settings.renameSetting("colors_mod3", "o_colors_mod3", "colors_mod3_rgb", "modcard3");
        settings.renameSetting("colors_mod4", "o_colors_mod4", "colors_mod4_rgb", "modcard4");
        settings.renameSetting("colors_modicon", "o_colors_modicon", "colors_modicon_rgb", "modicon");
        settings.renameSetting("colors_mod_settings_icon", "o_colors_mod_settings_icon", "colors_mod_settings_icon_rgb", "modsettings");
        getKeybind();
        Module::defaultConfig("core");
        setDef("editmenubind", (std::string) "L");
        setDef("custom_logo", false);
        setDef("globalText", (std::string) "ffffff", 1.f, false);
        setDef("headerText", (std::string)"ffffff", 1.f, false);
        setDef("settingsText", (std::string)"ffffff", 1.f, false);
        setDef("settingsSubtext", (std::string)"473b3d", 1.f, false);
        setDef("modNameText", (std::string) "8b767a", 1.f, false);
        setDef("modCardEnabled", (std::string) "188830", 1.f, false);
        setDef("modCardDisabled", (std::string) "7d1820", 1.f, false);
        setDef("primary1", (std::string) "ff233a", 1.f, false);
        setDef("primary2", (std::string) "ffffff", 1.f, false);
        setDef("primary3", (std::string) "9a6b72", 1.f, false);
        setDef("primary4", (std::string) "704b52", 1.f, false);
        setDef("secondary1", (std::string) "3f2a2d", 1.f, false);
        setDef("secondary2", (std::string) "201a1b", 1.f, false);
        setDef("secondary3", (std::string) "120e0f", 1.f, false);
        setDef("secondary4", (std::string) "1c1616", 1.f, false);
        setDef("secondary5", (std::string) "8b1b25", 1.f, false);
        setDef("secondary6", (std::string) "ff2438", 1.f, false);
        setDef("secondary7", (std::string) "943c3c", 1.f, false);
        setDef("secondary8", (std::string) "302728", 1.0f, false);
        setDef("modcard1", (std::string) "201a1b", 1.0f, false);
        setDef("modcard2", (std::string) "2f2022", 1.0f, false);
        setDef("modcard3", (std::string) "3f2a2d", 1.0f, false);
        setDef("modcard4", (std::string) "705d60", 1.0f, false);
        setDef("modicon", (std::string) "1A1313", 1.0f, false);
        setDef("flariallogo", (std::string) "FE4443", 1.0f, false);
        setDef("modsettings", (std::string) "FFFFFF", 1.0f, false);
        setDef("enabledRadioButton", (std::string) "D0A0A8", 1.0f, false);
        setDef("disabledRadioButton", (std::string) "FFFFFF", 1.0f, false);
        setDef("_overrideAlphaValues_", 1.f);
    }

    void settingsRender(float settingsOffset) override {
        if (getOps<std::string>("editmenubind").empty()) getOps<std::string>("editmenubind") = "L";

        initSettingsPage();

        this->addHeader("Keybinds");
        this->addKeybind("Open Menu Bind", "", "keybind", true);
        this->addKeybind("Edit Menu Bind", "", "editmenubind", true);

        this->extraPadding();

        this->addHeader("Logo");
        // this->addToggle("Custom logo", "", "custom_logo");
        // if (!getOps<bool>("custom_logo")) {
        this->addColorPicker("Logo Color", "Color of the client's logo", "flariallogo");
        // } else {
        //     this->addButton("", "To customize the logo, change the 'custom-logo.png' file in Flarial's Assets folder.", "Open Subfolder", [this]() {
        //         WinrtUtils::openSubFolder("Flarial\\assets");
        //     });
        // }

        addHeader("Colors");
        addColorPicker("Radio Button Icon Disabled", "", "enabledRadioButton");
        addColorPicker("Radio Button Icon Enabled", "", "disabledRadioButton");
        addColorPicker("Text Color", "", "globalText");
        addColorPicker("Header Text", "", "headerText");
        addColorPicker("Settings Text", "", "settingsText");
        addColorPicker("Settings Subtext", "", "settingsSubtext");
        addColorPicker("Module Name Text Color", "", "modNameText");
        addColorPicker("Enabled", "", "modCardEnabled");
        addColorPicker("Disabled", "", "modCardDisabled");
        addColorPicker("Primary 1", "Active elements, main color of sliders, bg color of enabled toggles", "primary1");
        addColorPicker("Primary 2", "Minor color of toggles and sliders, text Indicator color", "primary2");
        addColorPicker("Primary 3", "Color of inactive elements, unfilled slider bar color, inner color of color\npickers, background color of disabled toggles", "primary3");
        addColorPicker("Primary 4", "Base color of color pickers, hover color of dropdown childrens", "primary4");
        addColorPicker("Secondary 1", "Background color of settings", "secondary1");
        addColorPicker("Secondary 2", "Background color of navigation bar, tooltips, and buttons", "secondary2");
        addColorPicker("Secondary 3", "Background color of base Click GUI rectangle", "secondary3");
        addColorPicker("Secondary 4", "Background color of active Modules search bar", "secondary4");
        addColorPicker("Secondary 5", "Background color of navigation bar icons", "secondary5");
        addColorPicker("Secondary 6", "Background color of active navigation bar buttons", "secondary6");
        addColorPicker("Secondary 7", "Outline color of tooltips", "secondary7");
        addColorPicker("Secondary 8", "Navigation bar background deactivated Color", "secondary8");
        addColorPicker("Modcard 1", "Color of top rounded rectangle", "modcard1");
        addColorPicker("Modcard 2", "Color of bottom rounded rectangle", "modcard2");
        addColorPicker("Modcard 3", "Background color of module icons, outer color of module settings icon", "modcard3");
        addColorPicker("Modcard 4", "Inner color of module settings icon", "modcard4");
        addColorPicker("Modcard Icon", "Color of the mod's icon", "modicon");
        addColorPicker("Setting Icon Color", "Color of the mod's settings icon", "modsettings");
        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    static inline bool editmenu = false;

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event);

    //void fov(FOVEvent& event);

    void onKey(KeyEvent &event) {
        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()
        // #if !defined(__DEBUG__)
        if (SDK::getCurrentScreen() != "zoom_screen" &&
            SDK::getCurrentScreen() != "f3_screen" &&
            SDK::getCurrentScreen() != "hud_screen"
#if defined(__DEBUG__)
            && SDK::getServerIP() != "none"
#endif
        )
            return;

        if (event.getKey() == VK_CONTROL && event.getAction() == ActionType::Pressed) MC::holdingCTRL = true;
        else if (event.getKey() == VK_CONTROL && event.getAction() == ActionType::Released) MC::holdingCTRL = false;

        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key) && event.getAction() == ActionType::Pressed) {
#if !defined(__DEBUG__)
			if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen" && SDK::getCurrentScreen() != "f3_screen" && SDK::getCurrentScreen() != "zoom_screen") {
				WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
				this->active = false;
			    SDK::clientInstance->releaseMouse();
			}
			else {
#endif
            if (!editmenu) {
                if (!Client::settings.getSettingByName<bool>("nochaticon")->value)
                    Listen(this, PacketEvent, &ClickGUI::onPacketReceive)
                else
                    Deafen(this, PacketEvent, &ClickGUI::onPacketReceive);
                ModuleManager::cguiRefresh = true;
                keybindActions[0]({});
            }
#if !defined(__DEBUG__)
			}
#endif


            if (this->active) {
                MC::lastMouseScroll = MouseAction::Release;
                accumilatedPos = 0;
                accumilatedBarPos = 0;
                if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                    accumilatedPos = saved_acumilatedPos;
                    accumilatedBarPos = saved_acumilatedBarPos;
                }

                page.type = "normal";
                curr = "modules";
            } else {
                SDK::clientInstance->grabMouse(10);
# if defined(__DEBUG__)
                SDK::clientInstance->releaseMouse();
# endif

                if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                     saved_acumilatedPos = accumilatedPos;
                     saved_acumilatedBarPos = accumilatedBarPos;
                }

                FlarialGUI::ResetShit();
                Client::SaveSettings();
                Client::SavePrivate();
            }
        }

        // if clicked esc
        if ((event.getKey() == VK_ESCAPE && event.getAction() == ActionType::Released)) {
            if (!editmenu) {
                if (this->active) {
                    // exit ClickGUI
                    //WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                    if (
                        SDK::getCurrentScreen() == "hud_screen" ||
                        SDK::getCurrentScreen() == "f3_screen" ||
                        SDK::getCurrentScreen() == "zoom_screen"
                    )
                        SDK::clientInstance->grabMouse(10); // let mouse control the view
# if defined(__DEBUG__)
                    SDK::clientInstance->releaseMouse();
# endif

                    MC::lastMouseScroll = MouseAction::Release;
                    this->active = false;
                    page.type = "normal";
                    curr = "modules";

                    if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                        saved_acumilatedPos = accumilatedPos;
                        saved_acumilatedBarPos = accumilatedBarPos;
                    }

                    FlarialGUI::ResetShit();
                    Client::SaveSettings();
                    Client::SavePrivate();
                }
            } else {
                // switch from edit mode back to ClickGUI
                WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                MC::lastMouseScroll = MouseAction::Release;
                editmenu = false;
                this->active = true;
            }
        }

        if (this->active) {
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

            if (page.type == "normal" && curr == "modules" &&
                event.getAction() == ActionType::Pressed) {
                if (Client::settings.getSettingByName<bool>("autosearch")->value && !FlarialGUI::TextBoxes[0].isActive) {
                    FlarialGUI::TextBoxes[0].isActive = true;
                    event.setKey(MouseButton::None);
                    event.setAction(MouseAction::Release);
                }
                // you searching
                if (FlarialGUI::TextBoxes[0].isActive) {
                    FlarialGUI::scrollpos = 0;
                    FlarialGUI::barscrollpos = 0;

                    accumilatedPos = 0;
                    accumilatedBarPos = 0;
                    if (FlarialGUI::TextBoxes[0].text.empty() && Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                        accumilatedPos = saved_acumilatedPos;
                        accumilatedBarPos = saved_acumilatedBarPos;
                    }
                }
            }

            if (this->isAdditionalKeybind(event.keys,
                                          getOps<std::string>("editmenubind")) &&
                Module::isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>(
                                                         "editmenubind")->value)) {
                if (!editmenu) {
                    MC::lastMouseScroll = MouseAction::Release;
                    WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                    this->active = false;
                    FlarialGUI::Notify("Right click a module to directly go to their settings page.");
                    FlarialGUI::Notify("To disable this menu press ESC or " +
                                       getOps<std::string>("editmenubind"));
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
            MC::lastMouseScroll = MouseAction::Release;
            editmenu = false;
            this->active = true;
        }

        if (this->active || editmenu && SDK::getCurrentScreen() == "hud_screen") SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (this->active || editmenu) event.cancel(); // do not pass key event to the game
    }

    void onSetupAndRender(SetupAndRenderEvent &event) const {
        if (this->active || editmenu) SDK::clientInstance->releaseMouse();
    }

    void onMouse(MouseEvent &event) {
        GUIMouseListener::handleMouse(event);
        /*if (event.getMouseX() != 0) MC::mousePos.x = event.getMouseX();
        if (event.getMouseX() != 0) MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) {
            MC::held = true;
            if (event.getButton() == MouseButton::Left) MC::heldLeft = true;
            if (event.getButton() == MouseButton::Right) MC::heldRight = true;
        }
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) {
            MC::held = false;
            if (event.getButton() == MouseButton::Left) MC::heldLeft = false;
            if (event.getButton() == MouseButton::Right) MC::heldRight = false;
        }
        if (event.getButton() != MouseButton::None) MC::lastMouseButton = event.getButton();*/

        if (event.getButton() == MouseButton::Scroll) {
            if (editmenu == true) {
                if (!MC::scrollId) MC::scrollId = -1;
                MC::lastScrollId = MC::scrollId;
                MC::scrollId = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                MC::lastMouseScroll = event.getAction();
            } else {
                MC::lastMouseScroll = MouseAction::Release;
            }

            int scrollActionValue = static_cast<int>(event.getAction());

            if (scrollActionValue == static_cast<int>(MouseAction::ScrollUp)) {
                accumilatedPos += FlarialGUI::scrollposmodifier;
                accumilatedBarPos += FlarialGUI::barscrollposmodifier;
            } else if (scrollActionValue == static_cast<int>(MouseAction::ScrollDown)) {
                accumilatedPos -= FlarialGUI::scrollposmodifier;
                accumilatedBarPos -= FlarialGUI::barscrollposmodifier;
            } else {
                float sensitivity = 0.5f; // Adjust this value to control scroll speed for trackpad

                accumilatedPos += scrollActionValue * sensitivity;
                accumilatedBarPos += scrollActionValue * sensitivity * (FlarialGUI::barscrollposmodifier / FlarialGUI::scrollposmodifier);
            }
        }

        if (this->active) event.cancel();

#if !defined(__DEBUG__)
		if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen" && SDK::getCurrentScreen() != "f3_screen" && SDK::getCurrentScreen() != "zoom_screen") {
			if (this->active)
				this->active = false;
		}
#endif

        if ((this->active || editmenu) && (
                SDK::getCurrentScreen() == "hud_screen" ||
                SDK::getCurrentScreen() == "zoom_screen" ||
                SDK::getCurrentScreen() == "f3_screen"
            ))
            event.cancel(); // TODO: modules dont listen for canceled state!!!
    }
};
