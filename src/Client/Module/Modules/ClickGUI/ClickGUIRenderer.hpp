#pragma once

#include "../../../Client.hpp"
#include "../../../../SDK/SDK.hpp"
#include <format>
#include <windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../GUI/Engine/Constraints.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../Manager.hpp"
#include "../../../GUI/Engine/animations/fadeinout.hpp"
#include "GUIMouseListener.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Hook/Hooks/Render/ResizeHook.hpp"

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
#define colors_secondary6_rgb ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

struct PageType {
    std::string type = "normal";
    std::string module;
};

class ClickGUIRenderer : public Listener {
    Module *module;
    float baseHeightReal = 1.0f;
    float baseHeightActual = 0.00001f;
    float realBlurAmount = 0.00001f;
    std::string searchBarString;
public:
    static inline bool editmenu = false;

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event) override {
        if (ClientInstance::getTopScreenName() != "hud_screen") {
            FlarialGUI::NotifyHeartbeat();
            return;
        }

        float allahu = Constraints::RelativeConstraint(0.65);
        float akbar = Constraints::RelativeConstraint(0.25);
        Vec2<float> allahuakbar = Constraints::CenterConstraint(allahu, akbar, "y", 1.175, 1.175);
        // TODO: add inventory screen to onRender?
        // watermark
        if (ClientInstance::getTopScreenName() == "inventory_screen" ||
            SDK::currentScreen.find("chest") != std::string::npos)
            if (Client::settings.getSettingByName<bool>("watermark")->value)
                FlarialGUI::image(R"(\Flarial\assets\flarial-title.png)",
                                  D2D1::RectF(allahuakbar.x, allahuakbar.y, allahuakbar.x + allahu,
                                              allahuakbar.y + akbar));


        if (FlarialGUI::scrollposmodifier == 0) {
            FlarialGUI::scrollposmodifier = Constraints::RelativeConstraint(0.1f);
        }

        if (module->active) {

            FlarialGUI::lerp(baseHeightActual, 0.64f, 0.18f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, Client::settings.getSettingByName<float>("blurintensity")->value,
                             0.15f * FlarialGUI::frameFactor);

        } else {

            FlarialGUI::lerp(baseHeightReal, 0.01f, 0.22f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(baseHeightActual, 0.00001f, 0.30f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.15f * FlarialGUI::frameFactor);

            for (auto &box: FlarialGUI::TextBoxes) box.isActive = false;

        }

        if (realBlurAmount > 0.1) FlarialGUI::AllahBlur(realBlurAmount);


        if (SwapchainHook::init && baseHeightActual > 0.1) {

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.85);

            if (module->active) {
                if (ClickGUIRenderer::page.type == "settings" || curr == "settings") {

                    FlarialGUI::lerp(baseHeightReal, 0.50f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);

                    //FadeEffect::ApplyFadeOutEffect(0.015f * FlarialGUI::frameFactor, baseHeightReal, 0.35f);
                } else {
                    FlarialGUI::lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                    //FadeEffect::ApplyFadeInEffect(0.015f * FlarialGUI::frameFactor, 0.64f, baseHeightReal);
                }
            }

            float baseHeight = Constraints::RelativeConstraint(baseHeightReal);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth,
                                                               Constraints::RelativeConstraint(baseHeightReal), "r",
                                                               1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(60, 60);

            D2D1_COLOR_F basebaseRectangleColor = colors_secondary3_rgb ? FlarialGUI::rgbColor : colors_secondary3;
            basebaseRectangleColor.a = o_colors_secondary3;

            FlarialGUI::RoundedRect(center.x, center.y,
                                    basebaseRectangleColor, baseWidth,
                                    Constraints::RelativeConstraint(baseHeightReal), round.x, round.x);


            FlarialGUI::PushSize(center.x, center.y, baseWidth, Constraints::RelativeConstraint(baseHeightActual));

            /* Base Rectangle End */


            /* Nav Bar Start */

            float navigationBarWidth = Constraints::RelativeConstraint(1.309f);
            float navigationBarHeight = Constraints::RelativeConstraint(0.14f);
            float navx = Constraints::PercentageConstraint(0.008f, "left");
            float navy = Constraints::PercentageConstraint(0.009f, "top");
            round = Constraints::RoundingConstraint(50, 50);

            D2D1_COLOR_F navColor = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
            navColor.a = o_colors_secondary2;

            FlarialGUI::RoundedRect(navx, navy, navColor,
                                    navigationBarWidth, navigationBarHeight, round.x, round.x);


            FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

            /* Nav Bar End */

            /* Logo Start */


            float logoWidth = Constraints::RelativeConstraint(1.21);

            float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidth);
            float logoY = (navy + navigationBarHeight / 2.0f - logoWidth / 2.0f);

            if (!Client::settings.getSettingByName<bool>("noicons")->value)
                FlarialGUI::image(R"(\Flarial\assets\logo.png)",
                                  D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            FlarialGUI::Tooltip("easter egg", logoX, logoY, "Never gonna give you up", logoWidth, logoWidth);

            /* Logo End */

            /* tab buttons start */


            float RadioButtonWidth = Constraints::RelativeConstraint(0.185, "width");
            float RadioButtonHeight = Constraints::RelativeConstraint(0.5);

            float radioX = navx - Constraints::SpacingConstraint(-0.85, logoWidth);
            float radioY(navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            round = Constraints::RoundingConstraint(30, 30);

            D2D1_COLOR_F modTextCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
            modTextCol.a = o_colors_text;

            D2D1_COLOR_F mod6Col = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            mod6Col.a = o_colors_secondary6;

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(1, radioX, radioY, mod6Col,
                                                                                        modTextCol, L"Modules",
                                                                                        RadioButtonWidth,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "modules", curr)) {
                ClickGUIRenderer::curr = "modules";
                ClickGUIRenderer::page.type = "normal";
                FlarialGUI::ResetShit();

                FlarialGUI::scrollpos = 0;
                FlarialGUI::barscrollpos = 0;
                GUIMouseListener::accumilatedPos = 0;
                GUIMouseListener::accumilatedBarPos = 0;
            }


            logoWidth = Constraints::RelativeConstraint(0.21f);

            radioX -= Constraints::SpacingConstraint(-0.53f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.53f, logoWidth);

            float shit = Constraints::RelativeConstraint(0.33f);
            round = Constraints::RoundingConstraint(15, 15);

            D2D1_COLOR_F tabBgCol = colors_secondary5_rgb ? FlarialGUI::rgbColor : colors_secondary5;
            tabBgCol.a = o_colors_secondary5;

            FlarialGUI::RoundedRect(radioX + Constraints::SpacingConstraint(-0.15f, logoWidth),
                                    radioY + Constraints::SpacingConstraint(-0.12f, logoWidth),
                                    tabBgCol, shit, shit,
                                    round.x, round.x);

            radioX -= Constraints::SpacingConstraint(-0.125f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.15f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value)
                FlarialGUI::image(R"(\Flarial\assets\modules.png)",
                                  D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));


            radioX = navx - Constraints::SpacingConstraint(-0.85f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            round = Constraints::RoundingConstraint(30, 30);
            radioX += Constraints::SpacingConstraint(13.f, logoWidth);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(0, radioX, radioY,
                                                                                        mod6Col,
                                                                                        modTextCol, L"Settings",
                                                                                        RadioButtonWidth,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "settings",
                                                                                        ClickGUIRenderer::curr)) {
                FlarialGUI::TextBoxes[0].isActive = false;
                ClickGUIRenderer::curr = "settings";
            }

            const float h = Constraints::RelativeConstraint(0.42, "height");
            const float allahY = (navy + navigationBarHeight / 2.0f - h / 2.0f);
            ClickGUIElements::SearchBar(0, searchBarString, 12, Constraints::PercentageConstraint(0.022, "right"),
                                        allahY);

            radioX -= Constraints::SpacingConstraint(-0.53f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.53f, logoWidth);

            round = Constraints::RoundingConstraint(15, 15);

            FlarialGUI::RoundedRect(radioX + Constraints::SpacingConstraint(-0.15f, logoWidth),
                                    radioY + Constraints::SpacingConstraint(-0.12f, logoWidth),
                                    tabBgCol, shit, shit,
                                    round.x, round.x);

            radioX -= Constraints::SpacingConstraint(-0.125f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.15f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value)
                FlarialGUI::image(R"(\Flarial\assets\gear.png)",
                                  D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

            radioX = navx - Constraints::SpacingConstraint(-0.85f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);
            radioX += Constraints::SpacingConstraint(21.69f, logoWidth);
            round = Constraints::RoundingConstraint(30, 30);

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(2, radioX, radioY,
                                                                                        mod6Col,
                                                                                        modTextCol, L"Edit Menu",
                                                                                        RadioButtonWidth,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "editmenu",
                                                                                        ClickGUIRenderer::curr)) {
                module->active = false;
                FlarialGUI::Notify("To disable this menu press ESC or " +
                                   module->settings.getSettingByName<std::string>("editmenubind")->value);
                editmenu = true;
            }

            radioX -= Constraints::SpacingConstraint(-0.53f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.53f, logoWidth);

            round = Constraints::RoundingConstraint(15, 15);

            FlarialGUI::RoundedRect(radioX + Constraints::SpacingConstraint(-0.15f, logoWidth),
                                    radioY + Constraints::SpacingConstraint(-0.12f, logoWidth),
                                    tabBgCol, shit, shit,
                                    round.x, round.x);

            radioX -= Constraints::SpacingConstraint(-0.125f, logoWidth);
            radioY -= Constraints::SpacingConstraint(-0.15f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value)
                FlarialGUI::image(R"(\Flarial\assets\pencil.png)",
                                  D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

            /* tab buttons end */

            FlarialGUI::PopSize(); // Pops nav bar
            /* Mod Card Start */

            std::string e = ClickGUIRenderer::curr;

            if (ClickGUIRenderer::page.type == "normal") {

                if (e == "modules") {

                    float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                    float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                    Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.60,
                                                                          -0.52);

                    FlarialGUI::PushSize(center.x, center.y, baseWidth,
                                         Constraints::RelativeConstraint(baseHeightReal, "height", true));

                    float scrollWidth = Constraints::RelativeConstraint(1.12);
                    float scrollHeight = Constraints::RelativeConstraint(0.84);
                    Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0,
                                                                             1);

                    FlarialGUI::PopSize();

                    int i3 = 0;
                    float i2 = 0;

                    for (const auto& pair : ModuleManager::moduleMap) {
                        if ((++i3 % 3) == 0) {
                            i2 += Constraints::SpacingConstraint(0.8, modWidth);
                        }
                    }

                    FlarialGUI::ScrollBar(120, scrollcenter.y, 10, modHeight + i2, 2);

                    FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                    float xModifier = 0.0f;
                    float yModifier = 0.0f;

                    int i = 0;

                    static std::vector<Module *> modules = ModuleManager::getModules();

                    if (Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value)
                        std::sort(modules.begin(), modules.end(), compareEnabled);
                    else
                        std::sort(modules.begin(), modules.end(), compareNames);

                    for (Module *real: modules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 150) <
                                       center.y + Constraints::RelativeConstraint(baseHeightReal);
                        if (!searchBarString.empty()) {
                            std::string name = real->name;

                            for (char &c: name) {
                                c = std::tolower(c);
                            }

                            std::string search = searchBarString;

                            for (char &c: search) {
                                c = std::tolower(c);
                            }

                            if (name.starts_with(search) ||
                                name.find(search) != std::string::npos) {
                                ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, real,
                                                          real->icon, i, visible);
                                xModifier += Constraints::SpacingConstraint(1.09, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, real,
                                                      real->icon, i, visible);

                            xModifier += Constraints::SpacingConstraint(1.09, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();
                } else if (e == "settings") {

                    FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                    float rectX = Constraints::PercentageConstraint(0.01, "left");
                    float rectY = Constraints::PercentageConstraint(0.20, "top");
                    float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                    float rectHeight = Constraints::RelativeConstraint(0.755);

                    float scrollWidth = Constraints::RelativeConstraint(1.12);
                    float scrollHeight = Constraints::RelativeConstraint(1.3);
                    Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0, 1);

                    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
                    const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);
                    round = Constraints::RoundingConstraint(50, 50);

                    float anotherRectHeight = Constraints::RelativeConstraint(0.785);
                    float anotherRectWidth = Constraints::RelativeConstraint(0.981, "width");

                    D2D1_COLOR_F iRanOutOfNamesToCallTheseColors = colors_secondary2_rgb ? FlarialGUI::rgbColor
                                                                                         : colors_secondary2;
                    iRanOutOfNamesToCallTheseColors.a = o_colors_secondary2;

                    FlarialGUI::RoundedRect(rectX, rectY, iRanOutOfNamesToCallTheseColors, anotherRectWidth,
                                            anotherRectHeight, round.x, round.x);

                    D2D1_COLOR_F wtfCol = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                    wtfCol.a = o_colors_secondary1;

                    round = Constraints::RoundingConstraint(45, 45);
                    FlarialGUI::RoundedRect(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                            rectY + Constraints::SpacingConstraint(0.01, rectWidth), wtfCol, rectWidth,
                                            rectHeight, round.x, round.x);

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                         rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                         rectHeight);

                    rectX = Constraints::PercentageConstraint(0.019, "left");

                    FlarialGUI::ScrollBar(scrollWidth, scrollHeight, 270, 1000, 2);
                    FlarialGUI::SetScrollView(rectX, rectY + Constraints::SpacingConstraint(0.01, rectWidth),
                                              Constraints::RelativeConstraint(1.0, "width"),
                                              Constraints::RelativeConstraint(1.0, "height"));

                    rectY = Constraints::PercentageConstraint(0.10, "top");

                    FlarialGUI::TextBoxVisual(1, Client::settings.getSettingByName<std::string>("fontname")->value, 26,
                                              rectX, rectY, "Click GUI Font (Anything installed in your system)");

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::TextBoxVisual(2, Client::settings.getSettingByName<std::string>("mod_fontname")->value,
                                              26, rectX, rectY, "Modules Font (Anything installed in your system)");

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::Dropdown(0, rectX, rectY,
                                         std::vector<std::string>{"Default", "Cleartype", "Grayscale", "Aliased"},
                                         Client::settings.getSettingByName<std::string>("aliasingMode")->value,
                                         "Text Aliasing");
                    FlarialGUI::SetIsInAdditionalYMode();

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Eject keybind",
                                                    textWidth * 1.69f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(1.05, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);
                    FlarialGUI::KeybindSelector(0, rectX + FlarialGUI::SettingsTextWidth("Eject keybind "), rectY,
                                                Client::settings.getSettingByName<std::string>("ejectKeybind")->value);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Blur Intensity",
                                                    textWidth * 1.69f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(1.05, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    float percent = FlarialGUI::Slider(7, rectX + FlarialGUI::SettingsTextWidth("Blur Intensity "),
                                                       rectY, Client::settings.getSettingByName<float>(
                                    "blurintensity")->value, 25);

                    Client::settings.getSettingByName<float>("blurintensity")->value = percent;

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Chroma Speed",
                                                    textWidth * 1.69f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(1.05, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    percent = FlarialGUI::Slider(8, rectX + FlarialGUI::SettingsTextWidth("Chroma Speed "),
                                                 rectY, Client::settings.getSettingByName<float>(
                                    "rgb_speed")->value, 10);

                    Client::settings.getSettingByName<float>("rgb_speed")->value = percent;

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Chroma Saturation",
                                                    textWidth * 1.69f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(1.05, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    percent = FlarialGUI::Slider(9, rectX + FlarialGUI::SettingsTextWidth("Chroma Saturation "),
                                                 rectY, Client::settings.getSettingByName<float>(
                                    "rgb_saturation")->value, 1);

                    Client::settings.getSettingByName<float>("rgb_saturation")->value = percent;

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);

                    FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Chroma Value",
                                                    textWidth * 1.69f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(1.05, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    percent = FlarialGUI::Slider(10, rectX + FlarialGUI::SettingsTextWidth("Chroma Value "),
                                                 rectY, Client::settings.getSettingByName<float>(
                                    "rgb_value")->value, 1);

                    Client::settings.getSettingByName<float>("rgb_value")->value = percent;

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);


                    if (FlarialGUI::Toggle(0, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("killdx")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "killdx")->value = !Client::settings.getSettingByName<bool>("killdx")->value;
                    }


                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(1.2f, textWidth / 2.0f),
                                                    rectY,
                                                    L"Better Frames (No RTX) (Restart required)",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(1, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("vsync")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "vsync")->value = !Client::settings.getSettingByName<bool>("vsync")->value;
                    }

                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Vsync Disabler (Experimental)",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(9, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("disableanims")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "disableanims")->value = !Client::settings.getSettingByName<bool>(
                                "disableanims")->value;
                    }
                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Disable Animations",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(2, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("watermark")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "watermark")->value = !Client::settings.getSettingByName<bool>("watermark")->value;
                    }
                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Watermark In Inventories",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);


                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(8, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "enabledModulesOnTop")->value = !Client::settings.getSettingByName<bool>(
                                "enabledModulesOnTop")->value;
                    }
                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Enabled Modules on Top",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(7, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("centreCursor")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "centreCursor")->value = !Client::settings.getSettingByName<bool>(
                                "centreCursor")->value;
                    }

                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Centre Cursor",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);


                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(3, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("dlassets")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "dlassets")->value = !Client::settings.getSettingByName<bool>("dlassets")->value;
                    }

                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Re-Download Assets Every Restart",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(4, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("noicons")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "noicons")->value = !Client::settings.getSettingByName<bool>("noicons")->value;
                    }

                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"No Icons", Constraints::SpacingConstraint(4.5, textWidth),
                                                    textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    if (FlarialGUI::Toggle(5, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("noshadows")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "noshadows")->value = !Client::settings.getSettingByName<bool>("noshadows")->value;
                    }

                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"No Shadows", Constraints::SpacingConstraint(4.5, textWidth),
                                                    textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    rectY += Constraints::SpacingConstraint(0.35, textWidth);
                    FlarialGUI::FlarialTextWithFont(rectX + Constraints::SpacingConstraint(0.60, textWidth), rectY,
                                                    L"Anonymous Telemetry",
                                                    Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                                    DWRITE_TEXT_ALIGNMENT_LEADING,
                                                    Constraints::SpacingConstraint(0.95, textWidth),
                                                    DWRITE_FONT_WEIGHT_NORMAL);

                    if (FlarialGUI::Toggle(6, rectX, rectY,
                                           Client::settings.getSettingByName<bool>("anonymousApi")->value)) {

                        Client::settings.getSettingByName<bool>(
                                "anonymousApi")->value = !Client::settings.getSettingByName<bool>(
                                "anonymousApi")->value;
                    }

                    FlarialGUI::UnSetIsInAdditionalYMode();
                    FlarialGUI::UnsetScrollView();
                    FlarialGUI::PopSize();
                }

                /* Mod Card End */
            } else if (ClickGUIRenderer::page.type == "settings") {

                if (curr != "settings") {
                    ClickGUIRenderer::curr = "settings";
                    //FlarialGUI::TextBoxes[0].isActive = false;
                }

                FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                float rectX = Constraints::PercentageConstraint(0.01, "left");
                float rectY = Constraints::PercentageConstraint(0.20, "top");
                float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                float rectHeight = Constraints::RelativeConstraint(0.755);
                round = Constraints::RoundingConstraint(50, 50);

                float anotherRectHeight = Constraints::RelativeConstraint(0.785);
                float anotherRectWidth = Constraints::RelativeConstraint(0.981, "width");

                D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                colorThing.a = o_colors_secondary2;

                FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                        anotherRectWidth, anotherRectHeight, round.x, round.x);

                D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                bruv.a = o_colors_secondary1;

                D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                textCol.a = o_colors_text;

                round = Constraints::RoundingConstraint(40, 40);
                FlarialGUI::RoundedRect(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                        rectY + Constraints::SpacingConstraint(0.01, rectWidth),
                                        bruv, rectWidth,
                                        rectHeight, round.x, round.x);

                FlarialGUI::PopSize();

                FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                     rectHeight);

                if (!module->active)
                    FlarialGUI::SetScrollView(
                            rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                            rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth, rectHeight);

                ModuleManager::getModule(ClickGUIRenderer::page.module)->settingsRender();

                if (!module->active) FlarialGUI::UnsetScrollView();

                FlarialGUI::PopSize();

                if (!FlarialGUI::activeColorPickerWindows) {

                    float childHeight = Constraints::SpacingConstraint(0.5, rectHeight);
                    float childWidth = Constraints::SpacingConstraint(0.45, rectWidth);
                    std::pair<float, float> centered = centerChildRectangle(rectWidth, rectHeight, childWidth,
                                                                            childHeight);

                    round = Constraints::RoundingConstraint(25, 25);

                    FlarialGUI::RoundedRect(centered.first + rectX, rectHeight + rectY + centered.second +
                                                                    Constraints::RelativeConstraint(0.035), bruv,
                                            childWidth, childHeight, round.x, round.x);
                    FlarialGUI::RoundedHollowRect(centered.first + rectX, rectHeight + rectY + centered.second +
                                                                          Constraints::RelativeConstraint(0.035),
                                                  Constraints::RelativeConstraint(0.01, "height", true), colorThing,
                                                  childWidth, childHeight, round.x, round.x);

                    float buttonWidth = Constraints::RelativeConstraint(0.19f, "width");
                    float buttonHeight = Constraints::RelativeConstraint(0.1f, "height");
                    float spacingX = Constraints::RelativeConstraint(0.03);

                    std::pair<float, float> thingYes = centerChildRectangle(childWidth, childHeight, buttonWidth,
                                                                            buttonHeight);


                    if (FlarialGUI::RoundedButton(0, spacingX + centered.first + rectX,
                                                  thingYes.second + rectHeight + rectY + centered.second +
                                                  Constraints::RelativeConstraint(0.035), colorThing, textCol, L"Reset",
                                                  buttonWidth, buttonHeight, round.x, round.x)) {
                        auto currentModule = ModuleManager::getModule(ClickGUIRenderer::page.module);
                        bool wasEnabled = module->isEnabled();
                        currentModule->onDisable();
                        currentModule->loadDefaults();
                        FlarialGUI::ResetShit();
                        if(wasEnabled)
                            currentModule->onEnable();
                    }


                    if (FlarialGUI::RoundedButton(1, -spacingX + centered.first + rectX + childWidth - buttonWidth,
                                                  thingYes.second + rectHeight + rectY + centered.second +
                                                  Constraints::RelativeConstraint(0.035), colorThing, textCol,
                                                  L"Copy From", buttonWidth, buttonHeight, round.x, round.x)) {

                    }
                }
            }
            FlarialGUI::PopSize(); // Pops base rect
        }
        FlarialGUI::NotifyHeartbeat();
        FlarialGUI::displayToolTips();
    }

    void onKey(KeyEvent &event) override {

        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()

        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key) && event.getAction() == (int)ActionType::Pressed) {
            if (SDK::currentScreen != "hud_screen" && SDK::currentScreen != "pause_screen")
                module->active = false;
            else {
                module->active = !module->active;
            }

            if (module->active) {
                GUIMouseListener::accumilatedPos = 0;
                GUIMouseListener::accumilatedBarPos = 0;

                ClickGUIRenderer::page.type = "normal";
                ClickGUIRenderer::curr = "modules";
            }
            else {
                FlarialGUI::ResetShit();
                ModuleManager::SaveModulesConfig();
                Client::SaveSettings();
            }
        }

        // if clicked esc
        if ((event.getKey() == VK_ESCAPE && event.getAction() == (int) ActionType::Released)) {
            if (!editmenu) {
                if (module->active) {
                    // exit ClickGUI
                    SDK::clientInstance->grabMouse(); // let mouse control the view

                    module->active = false;
                    ClickGUIRenderer::page.type = "normal";
                    ClickGUIRenderer::curr = "modules";

                    FlarialGUI::ResetShit();
                    ModuleManager::SaveModulesConfig();
                    Client::SaveSettings();
                }
            } else {
                // switch from edit mode back to ClickGUI
                editmenu = false;
                module->active = true;
            }
        }

        if (module->active) {
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

            // auto search? TODO: make it optional
            if (ClickGUIRenderer::page.type == "normal" && ClickGUIRenderer::curr == "modules" &&
                event.getAction() == (int) ActionType::Pressed) {

                //FlarialGUI::TextBoxes[0].isActive = true;
                // you searching
                if (FlarialGUI::TextBoxes[0].isActive) {
                    FlarialGUI::scrollpos = 0;
                    FlarialGUI::barscrollpos = 0;
                    GUIMouseListener::accumilatedPos = 0;
                    GUIMouseListener::accumilatedBarPos = 0;

                }
            }

            if (module->isAdditionalKeybind(event.keys,
                                            module->settings.getSettingByName<std::string>("editmenubind")->value) &&
                    Module::isKeyPartOfAdditionalKeybind(event.key, module->settings.getSettingByName<std::string>(
                            "editmenubind")->value)) {

                if (!editmenu) {
                    module->active = false;
                    FlarialGUI::Notify("To disable this menu press ESC or " +
                                       module->settings.getSettingByName<std::string>("editmenubind")->value);
                    editmenu = true;
                }

            }
        // switch back to ClickGUI if clicked on Edit Mode bind
        } else if (editmenu && module->isAdditionalKeybind(event.keys, module->settings.getSettingByName<std::string>(
                "editmenubind")->value) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                module->settings.getSettingByName<std::string>(
                                                                                        "editmenubind")->value) ||
                editmenu && module->isKeybind(event.keys) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                                  module->settings.getSettingByName<std::string>(
                                                                                                          "editmenubind")->value)) {

            editmenu = false;
            module->active = true;

        }

        if (module->active || editmenu && SDK::currentScreen == "hud_screen")
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (module->active || editmenu)
            event.cancel(); // do not pass key event to the game

        FlarialGUI::inMenu = module->active;
    }

    void onMouse(MouseEvent &event) override {
        if(SDK::currentScreen != "hud_screen"){
            if(module->active)
                module->active = false;
        }

        if ((module->active || editmenu) && SDK::currentScreen == "hud_screen")
            event.cancel(); // TODO: modules dont listen for canceled state!!!

    }

    static bool compareEnabled(Module *&obj1, Module *&obj2) {
        return obj1->isEnabled() >
               obj2->isEnabled();
    }

    static bool compareNames(Module *&obj1, Module *&obj2) {
        return obj1->name < obj2->name;
    }

public:

    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
        ClickGUIRenderer::curr = "modules";
    }

    static inline PageType page;
    static inline std::string curr;

};
