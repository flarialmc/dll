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
#define colors_secondary6_rgb clickgui->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

#define colors_secondary8 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary8")->value)
#define o_colors_secondary8 clickgui->settings.getSettingByName<float>("o_colors_secondary8")->value
#define colors_secondary8_rgb clickgui->settings.getSettingByName<bool>("colors_secondary8_rgb")->value



struct PageType {
    std::string type = "normal";
    std::string module;
};

class ClickGUIRenderer : public Listener {
    Module *module;
    Module *ghostMainModule;
    float baseHeightReal = 0.f;
    float baseHeightActual = 0.00001f;
    float realBlurAmount = 0.00001f;
    float radioPushAmount1 = 0.0000001f;
    float radioPushAmount2 = 0.0000001f;
    float width1 = 0.000001f;
    float width2 = 0.000001f;
    float width3 = 0.000001f;
    std::string searchBarString;
public:
    static inline bool editmenu = false;

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event) override {
        float allahu = Constraints::RelativeConstraint(0.65);
        float akbar = Constraints::RelativeConstraint(0.25);
        Vec2<float> allahuakbar = Constraints::CenterConstraint(allahu, akbar, "y", 1.175, 1.175);
        // TODO: add inventory screen to onRender?
        // watermark
        if (SDK::getCurrentScreen() == "inventory_screen" || SDK::getCurrentScreen().find("chest") != std::string::npos)
            if (Client::settings.getSettingByName<bool>("watermark")->value)
                FlarialGUI::image(IDR_FLARIAL_TITLE_PNG,
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

            FlarialGUI::lerp(baseHeightReal, 0.0001f, 0.22f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(baseHeightActual, 0.00001f, 0.30f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.15f * FlarialGUI::frameFactor);

            for (auto &box: FlarialGUI::TextBoxes) box.isActive = false;

        }

        if (SwapchainHook::init && baseHeightActual > 0.1f) {

            this->module->settings.getSettingByName<bool>("enabled")->value = true;

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.81);

            if (module->active) {

                FlarialGUI::lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);

            }

            if(Client::settings.getSettingByName<float>("blurintensity")->value != 0.01f)
            Blur::RenderBlur(event.RTV, 3, realBlurAmount / 2);

            float baseHeight = Constraints::RelativeConstraint(baseHeightReal);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth,
                                                               Constraints::RelativeConstraint(baseHeightReal), "r",
                                                               1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(43, 43);
            Vec2<float> baseRound = round;

            D2D1_COLOR_F basebaseRectangleColor = colors_secondary3_rgb ? FlarialGUI::rgbColor : colors_secondary3;
            basebaseRectangleColor.a = o_colors_secondary3;

            FlarialGUI::RoundedRect(center.x, center.y,
                                    basebaseRectangleColor, baseWidth,
                                    Constraints::RelativeConstraint(baseHeightReal), round.x, round.x);


            FlarialGUI::PushSize(center.x, center.y, baseWidth, Constraints::RelativeConstraint(baseHeightActual));

            /* Base Rectangle End */


            /* Nav Bar Start */

            float navigationBarWidth = Constraints::RelativeConstraint(1.235f);
            float navigationBarHeight = Constraints::RelativeConstraint(0.124f);
            float navx = Constraints::PercentageConstraint(0.013f, "left");
            float navy = Constraints::PercentageConstraint(0.019f, "top");
            round = Constraints::RoundingConstraint(28, 28);

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
                FlarialGUI::image(IDR_LOGO_PNG,
                                  D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            FlarialGUI::Tooltip("easter egg", logoX, logoY, "Never gonna give you up", logoWidth, logoWidth);

            /* Logo End */

            /* tab buttons start */

            float shit = Constraints::RelativeConstraint(0.448f);

            float RadioButtonWidth = Constraints::RelativeConstraint(0.134, "width");
            float RadioButtonHeight = shit;

            if(curr == "modules") FlarialGUI::lerp(width1, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width1, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "settings") FlarialGUI::lerp(width2, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width2, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "editmenu") FlarialGUI::lerp(width3, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width3, shit, 0.15f * FlarialGUI::frameFactor);

            float radioX = navx - Constraints::SpacingConstraint(-0.85, logoWidth);
            float radioY(navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            D2D1_COLOR_F modTextCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
            modTextCol.a = o_colors_text;

            D2D1_COLOR_F mod6Col = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            mod6Col.a = o_colors_secondary6;
            static D2D1_COLOR_F tabBgCol = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            static D2D1_COLOR_F tabBgCol2 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            static D2D1_COLOR_F tabBgCol3 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;

            tabBgCol.a = o_colors_secondary6;
            tabBgCol2.a = o_colors_secondary6;
            tabBgCol3.a = o_colors_secondary6;


            //radiobutton of modules

            if(ClickGUIRenderer::curr != "modules") {
                tabBgCol = FlarialGUI::LerpColor(tabBgCol, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol = FlarialGUI::LerpColor(tabBgCol, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2(radioX, radioY + Constraints::SpacingConstraint(0.015f, logoWidth)), Vec2{width1, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(1, radioX, radioY, tabBgCol,
                                                                                        modTextCol, L"Modules",
                                                                                        width1,
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


            logoWidth = shit * 0.625f;

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(ClickGUIRenderer::curr == "modules") FlarialGUI::image(IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_FOLDER_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
            }

            // radiobutton of settings

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            if(ClickGUIRenderer::curr != "settings") {
                tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            radioX += Constraints::SpacingConstraint(3.24f, logoWidth);
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            radioPushAmount1 = Constraints::SpacingConstraint(0.9f, logoWidth) + width1;
            radioX += radioPushAmount1;

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{width2, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(0, radioX, radioY,
                                                                                        tabBgCol2,
                                                                                        modTextCol, L"Settings",
                                                                                        width2,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "settings",
                                                                                        ClickGUIRenderer::curr)) {
                FlarialGUI::TextBoxes[0].isActive = false;
                ClickGUIRenderer::curr = "settings";
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(ClickGUIRenderer::curr == "settings") FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_SETTINGS_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
            }

            const float h = Constraints::RelativeConstraint(0.42, "height");
            const float allahY = (navy + navigationBarHeight / 2.0f - h / 2.0f);
            ClickGUIElements::SearchBar(0, searchBarString, 12, Constraints::PercentageConstraint(0.022, "right"),
                                        allahY);

            // radiobutton of editmenu

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 5.69f, logoWidth) + width1 + width2;
            radioX += radioPushAmount2;
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            if(ClickGUIRenderer::curr != "editmenu") {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{width3, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(2, radioX, radioY,
                                                                                        tabBgCol3,
                                                                                        modTextCol, L"",
                                                                                        width3,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "editmenu",
                                                                                        ClickGUIRenderer::curr)) {
                module->active = false;
                FlarialGUI::Notify("To disable this menu press ESC or " +
                                   module->settings.getSettingByName<std::string>("editmenubind")->value);
                editmenu = true;
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(ClickGUIRenderer::curr == "editmenu") FlarialGUI::image(IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_STYLUS_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

            }

            /* tab buttons end */

            FlarialGUI::PopSize(); // Pops nav bar
            /* Mod Card Start */

            std::string e = ClickGUIRenderer::curr;

            if (ClickGUIRenderer::page.type == "normal") {

                if (e == "modules") {

                    float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                    float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                    Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.58,
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

                    for (Module *pModule: modules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 200) <
                                       center.y + Constraints::RelativeConstraint(baseHeightReal);

                        if (!searchBarString.empty()) {
                            std::string name = pModule->name;

                            for (char &c: name) {
                                c = (char)std::tolower(c);
                            }

                            std::string search = searchBarString;

                            for (char &c: search) {
                                c = (char)std::tolower(c);
                            }

                            if (name.starts_with(search) ||
                                name.find(search) != std::string::npos) {
                                ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, pModule,
                                                          pModule->icon, i, visible);
                                xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, pModule,
                                                      pModule->icon, i, visible);

                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();

                    //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.15f, center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                    FlarialGUI::ShadowRect(Vec2{center.x, center.y + navigationBarHeight}, Vec2{baseWidth, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, D2D1::ColorF(D2D1::ColorF::Black), 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, center.x + baseWidth, center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, D2D1::ColorF(D2D1::ColorF::Black), 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
                } else if (e == "settings") {


                    FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                    float rectX = Constraints::PercentageConstraint(0.015, "left");
                    float rectY = Constraints::PercentageConstraint(0.167, "top");
                    float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                    float rectHeight = Constraints::RelativeConstraint(0.85);
                    round = Constraints::RoundingConstraint(38.f, 38.5f);

                    float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                    float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                    D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                    colorThing.a = o_colors_secondary2;

                    FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                            anotherRectWidth, anotherRectHeight, round.x, round.x);

                    D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                    bruv.a = o_colors_secondary1;

                    D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                    textCol.a = o_colors_text;

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                         rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                         rectHeight);

                    Module* c = this->ghostMainModule;


                    float toggleX = Constraints::PercentageConstraint(0.019, "left");
                    float toggleY = Constraints::PercentageConstraint(0.10, "top");

                    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


                    FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
                    FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                              Constraints::RelativeConstraint(1.0, "width"),
                                              Constraints::RelativeConstraint(0.88f, "height"));

                    c->addHeader("Keybinds");
                    c->addKeybind("Eject Keybind", "When setting, hold the new bind for 2 seconds", Client::settings.getSettingByName<std::string>("ejectKeybind")->value);

                    c->extraPadding();

                    c->addHeader("Fonts");
                    c->addTextBox("ClickGUI", "", Client::settings.getSettingByName<std::string>("fontname")->value);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("gui_font_scale")->value, 10.f, 0.f, true);

                    c->addTextBox("Modules", "", Client::settings.getSettingByName<std::string>("mod_fontname")->value);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("modules_font_scale")->value, 10.f, 0.f, true);

                    c->addToggle("Override Font Weight", "", Client::settings.getSettingByName<bool>("overrideFontWeight")->value);
                    c->addDropdown("Font Weight", "Bold, Thin, etc.", {"Bold", "Normal", "SemiBold", "ExtraBold", "Medium", "Light", "ExtraLight"}, Client::settings.getSettingByName<std::string>("fontWeight")->value);

                    c->extraPadding();

                    c->addHeader("Rendering");
                    c->addToggle("Better Frames", "RTX Disabled, Restart Required.", Client::settings.getSettingByName<bool>("killdx")->value);
                    c->addToggle("V-SYNC Disabler", "Only works with Better Frames.", Client::settings.getSettingByName<bool>("vsync")->value);
                    c->addToggle("Disable Animations", "",  Client::settings.getSettingByName<bool>("disableanims")->value);
                    c->addSlider("UI Blur Intensity", "", Client::settings.getSettingByName<float>("blurintensity")->value, 25.f);
                    c->addSlider("Chroma / RGB Speed", "", Client::settings.getSettingByName<float>("rgb_speed")->value, 10.f);
                    c->addSlider("Chroma / RGB Saturation", "", Client::settings.getSettingByName<float>("rgb_saturation")->value, 1.f);
                    c->addSlider("Chroma / RGB Value", "", Client::settings.getSettingByName<float>("rgb_value")->value, 1.f);
                    c->addToggle("Inventory Watermark", "", Client::settings.getSettingByName<bool>("watermark")->value);
                    c->addToggle("Enabled Modules on Top", "", Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value);
                    c->addToggle("No UI Icons / Images", "Flarial only", Client::settings.getSettingByName<bool>("noicons")->value);
                    c->addToggle("Disable UI Shadows", "Flarial only",  Client::settings.getSettingByName<bool>("noshadows")->value);

                    c->extraPadding();

                    c->addHeader("Misc");
                    c->addToggle("Center Cursor", "Centers your cursor everytime you open your inventory, etc.", Client::settings.getSettingByName<bool>("centreCursor")->value);
                    c->addToggle("Anonymous on API", "Stay anonymous on Flarial's API.", Client::settings.getSettingByName<bool>("anonymousApi")->value);

                    c->resetPadding();

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

                float rectX = Constraints::PercentageConstraint(0.015, "left");
                float rectY = Constraints::PercentageConstraint(0.167, "top");
                float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                float rectHeight = Constraints::RelativeConstraint(0.85);
                round = Constraints::RoundingConstraint(38.f, 38.5f);

                float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                colorThing.a = o_colors_secondary2;

                FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                        anotherRectWidth, anotherRectHeight, round.x, round.x);

                D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                bruv.a = o_colors_secondary1;

                D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                textCol.a = o_colors_text;

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

                    float childHeight = Constraints::SpacingConstraint(0.3, rectHeight);
                    float childWidth = Constraints::SpacingConstraint(0.45, rectWidth);
                    std::pair<float, float> centered = centerChildRectangle(rectWidth, rectHeight, childWidth,
                                                                            childHeight);

                    round = Constraints::RoundingConstraint(25, 25);

                    float buttonWidth = Constraints::RelativeConstraint(0.19f, "width");
                    float buttonHeight = Constraints::RelativeConstraint(0.1f, "height");
                    float spacingX = Constraints::RelativeConstraint(0.03);

                    std::pair<float, float> thingYes = centerChildRectangle(childWidth, childHeight, buttonWidth,
                                                                            buttonHeight);


                    if (FlarialGUI::RoundedButton(0, spacingX + centered.first + rectX,
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol, L"Reset",
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
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol,
                                                  L"Copy From", buttonWidth, buttonHeight, round.x, round.x)) {

                    }
                }
            }
            FlarialGUI::PopSize(); // Pops base rect
        }

        if(realBlurAmount < 0.5f)  this->module->settings.getSettingByName<bool>("enabled")->value = false;

        FlarialGUI::displayToolTips();
        FlarialGUI::NotifyHeartbeat();
    }

    void onKey(KeyEvent &event) override {

        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()

        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key) && event.getAction() == (int)ActionType::Pressed) {
            if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen")
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

        if (module->active || editmenu && SDK::getCurrentScreen() == "hud_screen")
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (module->active || editmenu)
            event.cancel(); // do not pass key event to the game

        FlarialGUI::inMenu = module->active;
    }

    void onMouse(MouseEvent &event) override {
        if(SDK::getCurrentScreen() != "hud_screen"){
            if(module->active)
                module->active = false;
        }

        if ((module->active || editmenu) && SDK::getCurrentScreen() == "hud_screen")
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
        this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
        ClickGUIRenderer::curr = "modules";
    }

    static inline PageType page;
    static inline std::string curr;

};
