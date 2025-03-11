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
            if (ClickGUI::containsAny(String::removeNonAlphanumeric(String::removeColorCodes(pkt->message)))){ pkt->message = fmt::format("§f[{}FLARIAL§f]§r ", "§4") + pkt->message;}
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

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0, "height"));

        FlarialGUI::KeybindSelector(0, x, y, getKeybind());

        y += Constraints::SpacingConstraint(0.35, textWidth);

        /*FlarialGUI::FlarialTextWithFont(x, y, L"Edit Menu", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);*/

        FlarialGUI::KeybindSelector(1, x + Constraints::SpacingConstraint(0.8, textWidth), y,
                                    settings.getSettingByName<std::string>("editmenubind")->value);

        if (settings.getSettingByName<std::string>("editmenubind")->value.empty())
            settings.getSettingByName<std::string>("editmenubind")->value = "L";

        float rectY = Constraints::PercentageConstraint(0.35, "top");
        float rectX = x;

        float tooltipV = Constraints::RelativeConstraint(0.0285, "height", true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Text Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(10, rectX + FlarialGUI::SettingsTextWidth("Text Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_text")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_text_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Text Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_text_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Text Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the text in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"), tooltipV,
                            true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Enabled Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(11, rectX + FlarialGUI::SettingsTextWidth("Enabled Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_enabled")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_enabled_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Enabled Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_enabled_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Enabled Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the enabled button in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Disabled Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(12, rectX + FlarialGUI::SettingsTextWidth("Disabled Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_disabled")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_disabled_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Disabled Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_disabled_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Disabled Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the disabled button in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(13, rectX + FlarialGUI::SettingsTextWidth("Primary 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 1  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 1  ") + tooltipV * 4.125f, rectY,
                            "Color of active elements\nMain color of sliders\nBackground color of enabled toggles",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(14, rectX + FlarialGUI::SettingsTextWidth("Primary 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 2  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 2  ") + tooltipV * 4.125f, rectY,
                            "Minor color of toggles and sliders\nText Indicator color",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(15, rectX + FlarialGUI::SettingsTextWidth("Primary 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 3  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 3  ") + tooltipV * 4.125f, rectY,
                            "Color of inactive elements\nUnfilled slider bar color\nInner color of color pickers\nBackground color of disabled toggles",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(16, rectX + FlarialGUI::SettingsTextWidth("Primary 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 4  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 4  ") + tooltipV * 4.125f, rectY,
                            "Base color of color pickers\nHover color of dropdown childrens",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(17, rectX + FlarialGUI::SettingsTextWidth("Secondary 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 1  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 1  ") + tooltipV * 4.125f, rectY,
                            "Background color of settings", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(18, rectX + FlarialGUI::SettingsTextWidth("Secondary 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 2  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 2  ") + tooltipV * 4.125f, rectY,
                            "Background color of navigation bar, tooltips, and buttons",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(19, rectX + FlarialGUI::SettingsTextWidth("Secondary 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 3  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 3  ") + tooltipV * 4.125f, rectY,
                            "Background color of base Click GUI rectangle", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(20, rectX + FlarialGUI::SettingsTextWidth("Secondary 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 4  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 4  ") + tooltipV * 4.125f, rectY,
                            "Background color of active Modules search bar", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 5",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(21, rectX + FlarialGUI::SettingsTextWidth("Secondary 5 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary5")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary5_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 5  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary5_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 5  ") + tooltipV * 4.125f, rectY,
                            "Background color of navigation bar icons", FlarialGUI::SettingsTextWidth("(i)"), tooltipV,
                            true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 6",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(26, rectX + FlarialGUI::SettingsTextWidth("Secondary 6 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary6")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary6_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 6  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary6_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 6  ") + tooltipV * 4.125f, rectY,
                            "Background color of active navigation bar buttons", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 7",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(27, rectX + FlarialGUI::SettingsTextWidth("Secondary 7 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary7")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary7_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 7  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary7_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 7  ") + tooltipV * 4.125f, rectY,
                            "Outline color of tooltips", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(22, rectX + FlarialGUI::SettingsTextWidth("ModCard 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 1  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 1  ") + tooltipV * 4.125f, rectY,
                            "Color of top rounded rectangle", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(23, rectX + FlarialGUI::SettingsTextWidth("ModCard 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 2  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 2  ") + tooltipV * 4.125f, rectY,
                            "Color of bottom rounded rectangle", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(24, rectX + FlarialGUI::SettingsTextWidth("ModCard 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 3  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 3  ") + tooltipV * 4.125f, rectY,
                            "Background color of module icons\nOuter color of module settings icon",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(25, rectX + FlarialGUI::SettingsTextWidth("ModCard 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 4  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 4  ") + tooltipV * 4.125f, rectY,
                            "Inner color of module settings icon", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        // next element

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(10, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_text")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_text")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_text_rgb")->value);
        FlarialGUI::ColorPickerWindow(11, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_enabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_enabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_enabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(12, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_disabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_disabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_disabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(13, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(14, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(15, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(16, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(17, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(18, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(19, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(20, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(21, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary5")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary5")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary5_rgb")->value);
        FlarialGUI::ColorPickerWindow(26, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary6")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary6")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary6_rgb")->value);
        FlarialGUI::ColorPickerWindow(27, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary7")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary7")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary7_rgb")->value);
        FlarialGUI::ColorPickerWindow(22, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod1_rgb")->value);
        FlarialGUI::ColorPickerWindow(23, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod2_rgb")->value);
        FlarialGUI::ColorPickerWindow(24, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod3_rgb")->value);
        FlarialGUI::ColorPickerWindow(25, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod4_rgb")->value);
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