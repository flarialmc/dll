#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/APIUtils.hpp"
#include "Utils/WinrtUtils.hpp"
#include "Utils/PlatformUtils.hpp"
#include <atomic>
#include <chrono>
#include <algorithm>
#include <Modules/Misc/Input/GUIMouseListener.hpp>
#include "../../../GUI/Engine/Constraints.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../../Assets/Assets.hpp"

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
    std::atomic<bool> pendingClose = false;
    std::string pendingSearchString;
    bool pendingGrabMouse = false;
    bool pendingSaveScrollPos = false;
    float pendingSavedAccumPos = 0;
    float pendingSavedAccumBarPos = 0;
    float baseHeightReal = 0.f;
    float realBlurAmount = 0.00001f;
    float radioPushAmount1 = 0.0000001f;
    float radioPushAmount2 = 0.0000001f;
    float width1 = 0.000001f;
    float width2 = 0.000001f;
    float width3 = 0.000001f;
    std::string searchBarString;
    Module *ghostMainModule;

    std::string lastmesg; // Using this to prevent double watermarks in messages

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

    static constexpr uint8_t section1stPart{0xC2}, section2ndPart{0xA7};

    static constexpr auto roleColors = std::to_array<std::pair<std::string_view, std::string_view> >({
        {"Dev", "§b"},
        {"Staff", "§f"},
        {"Gamer", "§u"},
        {"Media", "§a"},
        {"Booster", "§d"},
        {"Supporter", "§5"},
        {"Regular", "§4"}
    });

    static inline D2D_COLOR_F getColor(const std::string &text);
    static D2D_COLOR_F getFormatColor(const std::string& colorName);

private:
    template<typename WordContainer>
    static std::optional<std::pair<std::string_view /*first word match*/, size_t /*text index*/> > findFirstOf(std::string_view text, WordContainer &&words);

    static size_t sanitizedToRawIndex(std::string_view raw, size_t sanIdx);

    // static std::string& getMutableTextForWatermark(TextPacket& pkt);

public:
    void onPacketReceive(PacketEvent &event);
    void onDrawText(DrawTextEvent &event);

    ClickGUI();

    void onSetup() override {
    }

    void onEnable() override {
    }

    void onDisable() override {
    }

    void terminate() override {
        Deafen(this, MouseEvent, &ClickGUI::onMouse);
        Deafen(this, KeyEvent, &ClickGUI::onKey);
        Deafen(this, RenderEvent, &ClickGUI::onRender);
        Deafen(this, PacketEvent, &ClickGUI::onPacketReceive);
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
        setDef("headerText", (std::string) "ffffff", 1.f, false);
        setDef("settingsText", (std::string) "ffffff", 1.f, false);
        setDef("settingsSubtext", (std::string) "473b3d", 1.f, false);
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

        // Format tag colors for HUD labels (used with {red}, {green}, etc. in label text)
        setDef("formatColor_red", (std::string) "ff0000", 1.0f, false);
        setDef("formatColor_green", (std::string) "00ff00", 1.0f, false);
        setDef("formatColor_blue", (std::string) "0000ff", 1.0f, false);
        setDef("formatColor_yellow", (std::string) "ffff00", 1.0f, false);
        setDef("formatColor_orange", (std::string) "ffaa00", 1.0f, false);
        setDef("formatColor_purple", (std::string) "aa55ff", 1.0f, false);
        setDef("formatColor_pink", (std::string) "ff55ff", 1.0f, false);
        setDef("formatColor_cyan", (std::string) "55ffff", 1.0f, false);
        setDef("formatColor_white", (std::string) "ffffff", 1.0f, false);
        setDef("formatColor_black", (std::string) "000000", 1.0f, false);
        setDef("formatColor_gray", (std::string) "aaaaaa", 1.0f, false);
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

        addHeader("Format Tag Colors");
        addElementText("Use {color} tags in HUD label text formats.", "Example: {red}Hello {reset}world!");
        extraPadding();
        addElementText("You can also use hex colors, for example: {#ff00ff}.", "Use {reset} to return to the module's text color.");
        extraPadding();
        addColorPicker("Red", "Color for {red} tag", "formatColor_red");
        addColorPicker("Green", "Color for {green} tag", "formatColor_green");
        addColorPicker("Blue", "Color for {blue} tag", "formatColor_blue");
        addColorPicker("Yellow", "Color for {yellow} tag", "formatColor_yellow");
        addColorPicker("Orange", "Color for {orange} tag", "formatColor_orange");
        addColorPicker("Purple", "Color for {purple} tag", "formatColor_purple");
        addColorPicker("Pink", "Color for {pink} tag", "formatColor_pink");
        addColorPicker("Cyan", "Color for {cyan} tag", "formatColor_cyan");
        addColorPicker("White", "Color for {white} tag", "formatColor_white");
        addColorPicker("Black", "Color for {black} tag", "formatColor_black");
        addColorPicker("Gray", "Color for {gray} tag", "formatColor_gray");

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    static inline bool editmenu = false;
    // True when the main ClickGUI menu or edit menu is open.
    // Mirrors this->active but accessible statically from hooks.
    static inline bool menuOpen = false;
    // True when the ClickGUI background blur is visually active.
    // HUD modules check this to skip rendering so they don't draw
    // crisp on top of the blur (creating a half-blurred, half-sharp look).
    static inline bool blurActive = false;
    // Opacity for HUD elements during blur transitions (1.0 = fully visible, 0.0 = hidden).
    // Fades inversely with the blur amount so HUD elements smoothly disappear.
    static inline float hudOpacity = 1.0f;

    // RAII guard that applies hudOpacity to all ImGui vertices drawn during its lifetime.
    // Place after the blurActive early-return check in any HUD render function.
    struct HudFadeGuard {
        int vtxStart;
        HudFadeGuard();
        ~HudFadeGuard();
        HudFadeGuard(const HudFadeGuard&) = delete;
        HudFadeGuard& operator=(const HudFadeGuard&) = delete;
    };

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event);

    //void fov(FOVEvent& event);

    void onKey(KeyEvent &event) {
        std::string &clickguiKey = getOps<std::string>("keybind");
        if (!this->active && clickguiKey.empty()) {
            clickguiKey = "k";
            FlarialGUI::Notify("To change it to a different key, go to ClickGUI settings or use \'.bind <key>\'");
            FlarialGUI::Notify("Your ClickGUI Keybind was unset, it has been reset to \'k\'.");
            Client::SaveSettings();
        }

        SettingType<std::string> *ejectKey = Client::settings.getSettingByName<std::string>("ejectKeybind");

        if (!clickguiKey.empty() && clickguiKey == ejectKey->value) {
            ejectKey->value = "";
            FlarialGUI::Notify("Your Eject key has been unset.");
            FlarialGUI::Notify("Your ClickGUI and Eject keybind was the same.");
            Client::SavePrivate();
            Client::LoadPrivate();
        }

        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()
        // #if !defined(__DEBUG__)
        if (SDK::getCurrentScreen() != "zoom_screen" &&
            SDK::getCurrentScreen() != "f3_screen" &&
            SDK::getCurrentScreen() != "hud_screen" &&
            SDK::getCurrentScreen() != "pause_screen"
#if defined(__DEBUG__)
            && SDK::getServerIP() != "none"
#endif
        )
            return;

        if (event.getKey() == VK_CONTROL && event.getAction() == ActionType::Pressed) MC::holdingCTRL = true;
        else if (event.getKey() == VK_CONTROL && event.getAction() == ActionType::Released) MC::holdingCTRL = false;

        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key) && event.getAction() == ActionType::Pressed) {
            bool wasActive = this->active;
#if !defined(__DEBUG__)
            if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen" && SDK::getCurrentScreen() != "f3_screen" && SDK::getCurrentScreen() != "zoom_screen") {
                WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                // Immediately hide cursor + set inactive, defer state cleanup to render thread
                if (wasActive) {
                    this->active = false;
                    menuOpen = false;
                    SDK::clientInstance->grabMouse(10);
                    pendingGrabMouse = false;
                    pendingSaveScrollPos = false;
                    pendingSearchString = searchBarString;
                    pendingClose = true;
                } else {
                    SDK::clientInstance->releaseMouse();
                }
            } else {
#endif
                if (!editmenu) {
                    if (!Client::settings.getSettingByName<bool>("nochaticon")->value) {
                        Listen(this, PacketEvent, &ClickGUI::onPacketReceive);
                    } else {
                        Deafen(this, PacketEvent, &ClickGUI::onPacketReceive);
                    }
                    ModuleManager::cguiRefresh = true;
                    if (wasActive) {
                        // Immediately hide cursor + set inactive, defer state cleanup to render thread
                        this->active = false;
                        menuOpen = false;
                        SDK::clientInstance->grabMouse(10);
                        pendingGrabMouse = false;
                        pendingSaveScrollPos = Client::settings.getSettingByName<bool>("saveScrollPos")->value;
                        if (pendingSaveScrollPos) {
                            pendingSavedAccumPos = accumilatedPos;
                            pendingSavedAccumBarPos = accumilatedBarPos;
                        }
                        pendingSearchString = searchBarString;
                        pendingClose = true;

                        Client::SaveSettings();
                        Client::SavePrivate();
                    } else {
                        // Opening - safe to toggle directly
                        keybindActions[0]({});
                    }
                }
#if !defined(__DEBUG__)
            }
#endif

            if (!wasActive && this->active) {
                // Just opened — release any held mouse buttons so in-game
                // clicks don't bleed into the GUI.
                menuOpen = true;
                MC::held = false;
                MC::heldLeft = false;
                MC::heldRight = false;
                MC::lastMouseScroll = MouseAction::Release;
                accumilatedPos = 0;
                accumilatedBarPos = 0;
                if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                    accumilatedPos = saved_acumilatedPos;
                    accumilatedBarPos = saved_acumilatedBarPos;
                }

                page.type = "normal";
                curr = "modules";
            }
        }

        // if clicked esc
        if ((event.getKey() == VK_ESCAPE && event.getAction() == ActionType::Released)) {
            if (!editmenu) {
                // If color picker windows are open, close them instead of the whole GUI
                // This prevents a crash from ResetUIState() modifying shared state
                // while the render thread is still using it
                if (this->active && FlarialGUI::activeColorPickerWindows > 0) {
                    for (auto& [key, cp] : FlarialGUI::ColorPickers) {
                        if (cp.isActive) {
                            cp.isActive = false;
                        }
                    }
                    FlarialGUI::activeColorPickerWindows = 0;
                    return;
                }
                if (this->active) {
                    // Immediately hide cursor + set inactive, defer state cleanup to render thread
                    this->active = false;
                    menuOpen = false;
                    if (SDK::getCurrentScreen() == "hud_screen" ||
                        SDK::getCurrentScreen() == "f3_screen" ||
                        SDK::getCurrentScreen() == "zoom_screen") {
                        SDK::clientInstance->grabMouse(10);
                    }
                    pendingGrabMouse = false;
                    pendingSaveScrollPos = Client::settings.getSettingByName<bool>("saveScrollPos")->value;
                    if (pendingSaveScrollPos) {
                        pendingSavedAccumPos = accumilatedPos;
                        pendingSavedAccumBarPos = accumilatedBarPos;
                    }
                    pendingSearchString = searchBarString;
                    pendingClose = true;

                    Client::SaveSettings();
                    Client::SavePrivate();
                }
            } else {
                // switch from edit mode back to ClickGUI
                WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                MC::lastMouseScroll = MouseAction::Release;
                editmenu = false;
                this->active = true;
                menuOpen = true;
            }
        }

        if (this->active && !pendingClose.load()) {
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

            // Check if any textbox is active to prevent Edit Menu from opening when typing
            bool anyTextBoxActive = std::any_of(FlarialGUI::TextBoxes.begin(), FlarialGUI::TextBoxes.end(), [](const auto &pair) { return pair.second.isActive; });

            if (page.type == "normal" && curr == "modules" && event.getAction() == ActionType::Pressed) {
                if (!isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>("editmenubind")->value) && !FlarialGUI::TextBoxes[0].text.empty()) {
                    // auto search logic
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
                };
            }

            if (((!anyTextBoxActive && curr == "settings") || curr == "modules") && !FlarialGUI::TextBoxes[0].isActive && isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>("editmenubind")->value)) {
                event.setKey(MouseButton::None);
                event.setAction(MouseAction::Release);
                if (!editmenu) {
                    MC::lastMouseScroll = MouseAction::Release;
                    WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                    this->active = false;
                    menuOpen = false;
                    FlarialGUI::Notify("Right click a module to directly go to their settings page.");
                    FlarialGUI::Notify("To disable this menu press ESC or " +
                                       getOps<std::string>("editmenubind"));
                    editmenu = true;
                }
            }
            // switch back to ClickGUI if clicked on Edit Mode bind
        } else if (editmenu && this->isAdditionalKeybind(event.keys, this->settings.getSettingByName<std::string>("editmenubind")->value) && isKeyPartOfAdditionalKeybind(event.key,
                                                                                                                             this->settings.getSettingByName<std::string>(
                                                                                                                                 "editmenubind")->value) ||
                   editmenu && this->isKeybind(event.keys) && isKeyPartOfAdditionalKeybind(event.key,
                                                                                                   this->settings.getSettingByName<std::string>(
                                                                                                       "editmenubind")->value)) {
            MC::lastMouseScroll = MouseAction::Release;
            editmenu = false;
            this->active = true;
            menuOpen = true;
        }

        if ((this->active && !pendingClose.load()) || editmenu && SDK::getCurrentScreen() == "hud_screen") SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (this->active || editmenu) event.cancel(); // do not pass key event to the game
    }

    static inline bool gdkCursorReleased = false;

    static HCURSOR getGdkCursorFromType(winrt::Windows::UI::Core::CoreCursorType type) {
        using namespace winrt::Windows::UI::Core;
        switch (type) {
            case CoreCursorType::Hand: return LoadCursor(nullptr, IDC_HAND);
            case CoreCursorType::IBeam: return LoadCursor(nullptr, IDC_IBEAM);
            case CoreCursorType::SizeWestEast: return LoadCursor(nullptr, IDC_SIZEWE);
            case CoreCursorType::SizeNorthSouth: return LoadCursor(nullptr, IDC_SIZENS);
            case CoreCursorType::SizeAll: return LoadCursor(nullptr, IDC_SIZEALL);
            case CoreCursorType::UniversalNo: return LoadCursor(nullptr, IDC_NO);
            case CoreCursorType::Wait: return LoadCursor(nullptr, IDC_WAIT);
            case CoreCursorType::Cross: return LoadCursor(nullptr, IDC_CROSS);
            default: return LoadCursor(nullptr, IDC_ARROW);
        }
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        if ((this->active && !pendingClose.load()) || editmenu) {
            if (PlatformUtils::isGDK()) {
                // On GDK, release mouse once
                if (!gdkCursorReleased) {
                    SDK::clientInstance->releaseMouse();
                    gdkCursorReleased = true;
                }
                // Keep cursor unclipped each frame
                ClipCursor(nullptr);
                // Force cursor visible each frame (game may hide it)
                while (ShowCursor(TRUE) < 0);
                // Set cursor based on what the GUI requested
                SetCursor(getGdkCursorFromType(WinrtUtils::currentCursorType));
            } else {
                SDK::clientInstance->releaseMouse();
            }

            // Clear movement inputs when menu is open to prevent player movement
            if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {
                auto handler = SDK::clientInstance->getLocalPlayer()->getHandler();
                handler.setForward(false);
                handler.setRawForward(false);
                handler.setBackward(false);
                handler.setRawBackward(false);
                handler.setLeft(false);
                handler.setRawLeft(false);
                handler.setRight(false);
                handler.setRawRight(false);
            }
        } else if (PlatformUtils::isGDK()) {
            gdkCursorReleased = false;
        }
    }

    void onMouse(MouseEvent &event) {
        // On GDK platform, get mouse position directly from the window
        if (PlatformUtils::isGDK() && (this->active || editmenu)) {
            if (Client::window) {
                POINT cursorPos;
                if (GetCursorPos(&cursorPos)) {
                    if (ScreenToClient(Client::window, &cursorPos)) {
                        MC::mousePos.x = static_cast<float>(cursorPos.x);
                        MC::mousePos.y = static_cast<float>(cursorPos.y);
                    }
                }
            }
            // Still process button state and other event data
            MC::mouseButton = event.getButton();
            MC::mouseAction = event.getAction();
            if (event.getButton() != MouseButton::None) MC::lastMouseButton = event.getButton();

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
        } else {
            // Use standard event-based mouse handling for UWP/other platforms
            GUIMouseListener::handleMouse(event);
        }

        if (event.getButton() == MouseButton::Scroll) {
            int scrollActionValue = static_cast<int>(event.getAction());

            if (editmenu == true) {
                if (!MC::scrollId) MC::scrollId = -1;
                MC::lastScrollId = MC::scrollId;
                MC::scrollId = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                MC::lastMouseScroll = event.getAction();
            } else {
                MC::lastMouseScroll = MouseAction::Release;
            }

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
            if (this->active) { this->active = false; menuOpen = false; }
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
