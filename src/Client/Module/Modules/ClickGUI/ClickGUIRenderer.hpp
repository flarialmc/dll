#pragma once

#include "../../../Client.hpp"
#include "../../../../SDK/SDK.hpp"
#include <format>
#include <Windows.h>
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

struct PageType {
    std::string type = "normal";
    std::string module;
};

class ClickGUIRenderer : public Listener {

    template <typename T>
    void lerp(T& a, const T& b, float t)
    {
        // Perform linear interpolation between a and b based on t
        float interpolatedValue = a + (b - a) * t;

        // Round up the interpolated value to three decimal places
        float roundedValue = interpolatedValue;

        // Assign the rounded value back to 'a'
        a = roundedValue;
    }

    Module* module;
    std::string curr;
    float baseHeightReal = 0.64f;
    float baseHeightActual = 0.00001f;
    float realBlurAmount = 0.00001f;
public:
    static inline bool editmenu = false;

    void onRender(RenderEvent &event) override {

            float allahu = Constraints::RelativeConstraint(0.65);
            float akbar = Constraints::RelativeConstraint(0.25);
            Vec2<float> allahuakbar = Constraints::CenterConstraint(allahu, akbar, "y", 1.175, 1.175);

            // watermark
            if (SDK::clientInstance->getTopScreenName() == "inventory_screen" ||
                SDK::CurrentScreen.find("chest") != std::string::npos)
                FlarialGUI::Image("\\Flarial\\assets\\flarial-title.png",
                                  D2D1::RectF(allahuakbar.x, allahuakbar.y, allahuakbar.x + allahu,
                                              allahuakbar.y + akbar));


            if (FlarialGUI::scrollposmodifier == 0) {
                FlarialGUI::scrollposmodifier = Constraints::RelativeConstraint(0.1f);
            }

            if (module->settings.getSettingByName<bool>("enabled")->value) {


                lerp(baseHeightActual, 0.64f, 0.18f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                lerp(realBlurAmount, Client::settings.getSettingByName<float>("blurintensity")->value, 0.15f * FlarialGUI::frameFactor);

            } else {

                lerp(baseHeightReal, 0.01f, 0.22f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                lerp(baseHeightActual, 0.00001f, 0.30f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                lerp(realBlurAmount, 0.00001f, 0.15f * FlarialGUI::frameFactor);

        }

            if(realBlurAmount > 0.01) FlarialGUI::AllahBlur(realBlurAmount);

            if (SwapchainHook::init && baseHeightActual > 0.01) {



                /* Base Rectangle Start */

                float baseWidth = Constraints::RelativeConstraint(0.85);

                if (module->settings.getSettingByName<bool>("enabled")->value){
                    if (ClickGUIRenderer::page.type == "settings" || curr == "settings") {

                        lerp(baseHeightReal, 0.35f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                        //FadeEffect::ApplyFadeOutEffect(0.015f * FlarialGUI::frameFactor, baseHeightReal, 0.35f);
                    } else {
                        lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
                        //FadeEffect::ApplyFadeInEffect(0.015f * FlarialGUI::frameFactor, 0.64f, baseHeightReal);
                    }
                }

                float baseHeight = Constraints::RelativeConstraint(baseHeightReal);

                Vec2<float> center = Constraints::CenterConstraint(baseWidth,
                                                                   Constraints::RelativeConstraint(baseHeightReal), "r",
                                                                   1, 1);
                Vec2<float> round = Constraints::RoundingConstraint(50, 50);

                FlarialGUI::ShadowRect(D2D1::RoundedRect(
                        D2D1::RectF(center.x, center.y, center.x + Constraints::SpacingConstraint(1.02f, baseWidth),
                                    center.y + Constraints::RelativeConstraint(baseHeightReal * 1.02f)), round.x,
                        round.x));
                FlarialGUI::RoundedRect(center.x, center.y,
                                        D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), baseWidth,
                                        Constraints::RelativeConstraint(baseHeightReal), round.x, round.x);
                FlarialGUI::PushSize(center.x, center.y, baseWidth, Constraints::RelativeConstraint(baseHeightActual));

                /* Base Rectangle End */


                /* Nav Bar Start */

                float navigationBarWidth = Constraints::RelativeConstraint(1.309f);
                float navigationBarHeight = Constraints::RelativeConstraint(0.14f);
                float navx = Constraints::PercentageConstraint(0.008f, "left");
                float navy = Constraints::PercentageConstraint(0.009f, "top");
                round = Constraints::RoundingConstraint(50, 50);

                FlarialGUI::RoundedRect(navx, navy, D2D1::ColorF(32.0f / 255.0f, 26.0f / 255.0f, 27.0f / 255.0f),
                                        navigationBarWidth, navigationBarHeight, round.x, round.x);


                FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

                /* Nav Bar End */

                /* Logo Start */


                float logoWidth = Constraints::RelativeConstraint(1.21);

                float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidth);
                float logoY = (navy + navigationBarHeight / 2.0f - logoWidth / 2.0f);

                FlarialGUI::Image("\\Flarial\\assets\\logo.png",
                                  D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

                /* Logo End */

                /* tab buttons start */


                float RadioButtonWidth = Constraints::RelativeConstraint(0.185, "width");
                float RadioButtonHeight = Constraints::RelativeConstraint(0.5);

                float radioX = navx - Constraints::SpacingConstraint(-0.85, logoWidth);
                float radioY(navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

                round = Constraints::RoundingConstraint(30, 30);

                if (FlarialGUI::RoundedRadioButton(1, radioX, radioY,
                                                   D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                                   D2D1::ColorF(D2D1::ColorF::White), L"Modules", RadioButtonWidth,
                                                   RadioButtonHeight, round.x, round.x, "modules", curr)) {
                    this->curr = "modules";
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

                FlarialGUI::RoundedRect(radioX + Constraints::SpacingConstraint(-0.15f, logoWidth),
                                        radioY + Constraints::SpacingConstraint(-0.12f, logoWidth),
                                        D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f), shit, shit,
                                        round.x, round.x);

                radioX -= Constraints::SpacingConstraint(-0.125f, logoWidth);
                radioY -= Constraints::SpacingConstraint(-0.15f, logoWidth);
                FlarialGUI::Image("\\Flarial\\assets\\modules.png",
                                  D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));


                radioX = navx - Constraints::SpacingConstraint(-0.85f, logoWidth);
                radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

                round = Constraints::RoundingConstraint(30, 30);
                radioX += Constraints::SpacingConstraint(13.f, logoWidth);
                if (FlarialGUI::RoundedRadioButton(0, radioX, radioY,
                                                   D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                                   D2D1::ColorF(D2D1::ColorF::White), L"Settings", RadioButtonWidth,
                                                   RadioButtonHeight, round.x, round.x, "settings", this->curr))
                    this->curr = "settings";;

                radioX -= Constraints::SpacingConstraint(-0.53f, logoWidth);
                radioY -= Constraints::SpacingConstraint(-0.53f, logoWidth);

                round = Constraints::RoundingConstraint(15, 15);

                FlarialGUI::RoundedRect(radioX + Constraints::SpacingConstraint(-0.15f, logoWidth),
                                        radioY + Constraints::SpacingConstraint(-0.12f, logoWidth),
                                        D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f), shit, shit,
                                        round.x, round.x);

                radioX -= Constraints::SpacingConstraint(-0.125f, logoWidth);
                radioY -= Constraints::SpacingConstraint(-0.15f, logoWidth);
                FlarialGUI::Image("\\Flarial\\assets\\gear.png",
                                  D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

                /* tab buttons end */

                FlarialGUI::PopSize(); // Pops nav bar
                /* Mod Card Start */

                std::string e = this->curr;

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

                        for (Module *real: ModuleManager::modules) {
                            if ((++i3 % 3) == 0) {
                                i2 += Constraints::SpacingConstraint(0.8, modWidth);
                            }
                        }

                        FlarialGUI::ScrollBar(120, scrollcenter.y, 10,modHeight + i2, 2);

                        FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                        float xModifier = 0.0f;
                        float yModifier = 0.0f;

                        int i = 0;
                        for (Module *real: ModuleManager::modules) {
                            ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, real,
                                                      real->icon, i);

                            xModifier += Constraints::SpacingConstraint(1.09, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }
                        }

                        FlarialGUI::UnsetScrollView();
                    }
                    else if (e == "settings") {

                        FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                        float rectX = Constraints::PercentageConstraint(0.01, "left");
                        float rectY = Constraints::PercentageConstraint(0.32, "top");
                        float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                        float rectHeight = Constraints::RelativeConstraint(0.55);
                        float scrollWidth = Constraints::RelativeConstraint(1.12);
                        float scrollHeight = Constraints::RelativeConstraint(1.3);
                        Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0, 1);

                        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
                        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);
                        round = Constraints::RoundingConstraint(50, 50);

                        float anotherRectHeight = Constraints::RelativeConstraint(0.60);
                        float anotherRectWidth = Constraints::RelativeConstraint(0.981, "width");

                        FlarialGUI::RoundedRect(rectX, rectY, D2D1::ColorF(32.0f / 255.0f, 26.0f / 255.0f, 27.0f / 255.0f), anotherRectWidth, anotherRectHeight, round.x, round.x);

                        round = Constraints::RoundingConstraint(45, 45);
                        FlarialGUI::RoundedRect(rectX + Constraints::SpacingConstraint(0.0085, rectWidth), rectY + Constraints::SpacingConstraint(0.01, rectWidth), D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), rectWidth, rectHeight, round.x, round.x);

                        FlarialGUI::PopSize();

                        FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth), rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth, rectHeight);

                        FlarialGUI::ScrollBar(scrollWidth, scrollHeight, 140, 100, 2);
                        FlarialGUI::SetScrollView(rectX, rectY + Constraints::SpacingConstraint(0.01, rectWidth), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(1.0, "height"));

                        FlarialGUI::TextBoxVisual(0, Client::settings.getSettingByName<std::string>("fontname")->value, 26, Constraints::PercentageConstraint(0.019, "left"), Constraints::PercentageConstraint(0.10, "top"), "Font (Anything installed in your system)");

                        FlarialGUI::FlarialTextWithFont(Constraints::PercentageConstraint(0.019, "left"), Constraints::PercentageConstraint(0.40, "top"), FlarialGUI::to_wide("Blur Intensity").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

                        float percent = FlarialGUI::Slider(7, Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(1.06, textWidth),
                            Constraints::PercentageConstraint(0.40, "top"),
                            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                            D2D1::ColorF(D2D1::ColorF::White), Client::settings.getSettingByName<float>("blurintensity")->value, 25);

                        Client::settings.getSettingByName<float>("blurintensity")->value = percent;

                        rectY += Constraints::SpacingConstraint(1.0, textWidth);
                        if (FlarialGUI::Toggle(0, Constraints::PercentageConstraint(0.019, "left"), rectY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), Client::settings.getSettingByName<bool>("killdx")->value)) {

                            Client::settings.getSettingByName<bool>("killdx")->value = !Client::settings.getSettingByName<bool>("killdx")->value;
                        }

                        FlarialGUI::FlarialTextWithFont(Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(1.2f, textWidth / 2.0f), rectY, L"Better Frames and less Input Lag (No RTX) (Restart required)", D2D1::ColorF(D2D1::ColorF::White), Constraints::SpacingConstraint(4.5, textWidth), textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.95, textWidth));

                        rectY += Constraints::SpacingConstraint(0.35, textWidth);
                        if (FlarialGUI::Toggle(1, Constraints::PercentageConstraint(0.019, "left"), rectY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), Client::settings.getSettingByName<bool>("vsync")->value)) {

                            Client::settings.getSettingByName<bool>("vsync")->value = !Client::settings.getSettingByName<bool>("vsync")->value;
                        }

                        FlarialGUI::FlarialTextWithFont(Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(0.60, textWidth), rectY, L"Vsync Disabler (Experimental)", D2D1::ColorF(D2D1::ColorF::White), Constraints::SpacingConstraint(4.5, textWidth), textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.95, textWidth));

                        rectY += Constraints::SpacingConstraint(0.35, textWidth);

                        FlarialGUI::FlarialTextWithFont(Constraints::PercentageConstraint(0.019, "left"), rectY, FlarialGUI::to_wide("MC GUI Scale").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

                        percent = FlarialGUI::Slider(8, Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(1.06, textWidth),
                                                           rectY,
                                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                                           D2D1::ColorF(D2D1::ColorF::White), SDK::clientInstance->guiData->GuiScale, 5);

                        SDK::clientInstance->guiData->GuiScale = percent;


                        FlarialGUI::UnsetScrollView();

                        FlarialGUI::PopSize();
                }

                    /* Mod Card End */
                } else if (ClickGUIRenderer::page.type == "settings") {

                    this->curr = "settings";

                    FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                    float rectX = Constraints::PercentageConstraint(0.01, "left");
                    float rectY = Constraints::PercentageConstraint(0.32, "top");
                    float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                    float rectHeight = Constraints::RelativeConstraint(0.55);
                    round = Constraints::RoundingConstraint(50, 50);

                    float anotherRectHeight = Constraints::RelativeConstraint(0.60);
                    float anotherRectWidth = Constraints::RelativeConstraint(0.981, "width");

                    FlarialGUI::RoundedRect(rectX, rectY, D2D1::ColorF(32.0f / 255.0f, 26.0f / 255.0f, 27.0f / 255.0f),
                                            anotherRectWidth, anotherRectHeight, round.x, round.x);

                    round = Constraints::RoundingConstraint(45, 45);
                    FlarialGUI::RoundedRect(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                            rectY + Constraints::SpacingConstraint(0.01, rectWidth),
                                            D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), rectWidth,
                                            rectHeight, round.x, round.x);

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                         rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                         rectHeight);

                    if (!module->settings.getSettingByName<bool>("enabled")->value) FlarialGUI::SetScrollView(
                            rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                            rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth, rectHeight);

                    ModuleManager::getModule(ClickGUIRenderer::page.module)->SettingsRender();

                    if (!module->settings.getSettingByName<bool>("enabled")->value) FlarialGUI::UnsetScrollView();

                    FlarialGUI::PopSize();

                }

                FlarialGUI::PopSize(); // Pops base rect


            }

        FlarialGUI::NotifyHeartbeat();
    }

    void onKey(KeyEvent &event) override {

        if(module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) {

            module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;

            if(!module->settings.getSettingByName<bool>("enabled")->value && SDK::CurrentScreen == "hud_screen") SDK::clientInstance->grabMouse();

            if(module->settings.getSettingByName<bool>("enabled")->value) {
                GUIMouseListener::accumilatedPos = 0;
                GUIMouseListener::accumilatedBarPos = 0;
            }

            if(SDK::CurrentScreen != "hud_screen" && SDK::CurrentScreen != "pause_screen")
             module->settings.getSettingByName<bool>("enabled")->value = false;

            ClickGUIRenderer::page.type = "normal";
            ClickGUIRenderer::curr = "modules";

            if(!module->settings.getSettingByName<bool>("enabled")->value) {

                FlarialGUI::ResetShit();
                ModuleManager::SaveModulesConfig();
                Client::SaveSettings();

            }
        }


        if(event.GetKey() == VK_ESCAPE && event.GetAction() == (int)ActionType::RELEASED) {

            if(!editmenu) {
                if(module->settings.getSettingByName<bool>("enabled")->value){
                    if (SDK::CurrentScreen == "hud_screen") SDK::clientInstance->grabMouse();

                    module->settings.getSettingByName<bool>("enabled")->value = false;
                    ClickGUIRenderer::page.type = "normal";
                    ClickGUIRenderer::curr = "modules";
                }
            } else {
                editmenu = false;
                module->settings.getSettingByName<bool>("enabled")->value = true;
            }

            if(!module->settings.getSettingByName<bool>("enabled")->value && !editmenu) {

                FlarialGUI::ResetShit();
                ModuleManager::SaveModulesConfig();
                Client::SaveSettings();

            }

        }

        if(module->settings.getSettingByName<bool>("enabled")->value) {

            if(SDK::CurrentScreen == "hud_screen") SDK::clientInstance->releaseMouse();

            if(module->IsAllahKeybind(event.keys, module->settings.getSettingByName<std::string>("editmenubind")->value) && module->IsKeyPartOfAllahKeybind(event.key, module->settings.getSettingByName<std::string>("editmenubind")->value)) {

                if(!editmenu) {
                    module->settings.getSettingByName<bool>("enabled")->value = false;
                    FlarialGUI::Notify("To disable this menu press ESC or " + module->settings.getSettingByName<std::string>("editmenubind")->value);
                    editmenu = true;
                }

            }

        } else if (editmenu && module->IsAllahKeybind(event.keys, module->settings.getSettingByName<std::string>("editmenubind")->value) && module->IsKeyPartOfAllahKeybind(event.key, module->settings.getSettingByName<std::string>("editmenubind")->value) || editmenu && module->IsKeybind(event.keys) && module->IsKeyPartOfAllahKeybind(event.key, module->settings.getSettingByName<std::string>("editmenubind")->value)) {

            editmenu = false;
            module->settings.getSettingByName<bool>("enabled")->value = true;

        }

        if(module->settings.getSettingByName<bool>("enabled")->value || editmenu)
        event.setCancelled(true);

    }

public:
    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
        this->curr = "modules";
    }

    static inline PageType page;
};
