#include "ClickGUI.hpp"

#include <random>
#include <Scripting/ScriptManager.hpp>
#include <Scripting/ModuleScript.hpp>

#include "Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp"

std::chrono::time_point<std::chrono::high_resolution_clock> ClickGUI::favoriteStart;


void ClickGUI::onRender(RenderEvent &event) {

        float allahu = Constraints::RelativeConstraint(0.65);
        float akbar = Constraints::RelativeConstraint(0.25);

        if(editmenu) {
            D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::Black);
            c.a = 0.75f;
            FlarialGUI::RoundedRect(0, 0, c, MC::windowSize.x, MC::windowSize.y, 0, 0);
        }
        Vec2<float> allahuakbar = Constraints::CenterConstraint(allahu, akbar, "y", 1.175, 1.175);
        // TODO: add inventory screen to onRender?
        // watermark
        if (SDK::getCurrentScreen() == "inventory_screen" || SDK::getCurrentScreen().find("chest") != std::string::npos)
            if (Client::settings.getSettingByName<bool>("watermark")->value)
                FlarialGUI::image(IDR_FLARIAL_TITLE_PNG, D2D1::RectF(allahuakbar.x, allahuakbar.y, allahuakbar.x + allahu, allahuakbar.y + akbar));


        if (FlarialGUI::scrollposmodifier == 0) {
            FlarialGUI::scrollposmodifier = Constraints::RelativeConstraint(0.1f);
        }

        if (this->active) {

            FlarialGUI::lerp(baseHeightActual, 0.64f, 0.18f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, Client::settings.getSettingByName<float>("blurintensity")->value,
                             0.15f * FlarialGUI::frameFactor);

        } else {

            FlarialGUI::lerp(baseHeightReal, 0.0001f, 0.22f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(baseHeightActual, 0.00001f, 0.30f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.15f * FlarialGUI::frameFactor);

            for (auto &box: FlarialGUI::TextBoxes) box.second.isActive = false;

        }

        if (SwapchainHook::init && baseHeightActual > 0.1f) {

            setEnabled(true);

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.81);

            if (this->active) {

                FlarialGUI::lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);

            }

            if(Client::settings.getSettingByName<float>("blurintensity")->value > 0.01f)
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
            float logoWidthButReal = Constraints::RelativeConstraint(0.5);

            float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidthButReal) + (logoWidthButReal * 0.6);
            float logoY = (navy + navigationBarHeight / 2.0f - logoWidthButReal / 2.0f);

            D2D1_COLOR_F fLARIALlOGO = colors_FlarialLogo_rgb ? FlarialGUI::rgbColor : colors_FlarialLogo;
            fLARIALlOGO.a = o_colors_FlarialLogo;

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if (this->settings.getSettingByName<bool>("custom_logo")->value && std::filesystem::exists(Utils::getRoamingPath() + "\\Flarial\\assets\\custom-logo.png")) {
                    FlarialGUI::image("Flarial\\assets\\custom-logo.png",
                                          D2D1::RectF(logoX, logoY, logoX + logoWidthButReal,
                                                      logoY + logoWidthButReal));

                } else {
                    FlarialGUI::image(IDR_WHITE_LOGO_PNG, D2D1::RectF(logoX, logoY, logoX + logoWidthButReal, logoY + logoWidthButReal), "PNG", true, FlarialGUI::D2DColorToImColor(fLARIALlOGO));
                }
            }
            FlarialGUI::Tooltip("easter egg", logoX, logoY, "Never gonna give you up", logoWidthButReal, logoWidthButReal);

            /* Logo End */

            /* tab buttons start */

            D2D1_COLOR_F RadioButtonEnabled = colors_radiobutton_enabled_rgb ? FlarialGUI::rgbColor : colors_radiobutton_enabled;
            RadioButtonEnabled.a = o_colors_radiobutton_enabled;

            D2D1_COLOR_F RadioButtonDisabled = colors_radiobutton_disabled_rgb ? FlarialGUI::rgbColor : colors_radiobutton_disabled;
            RadioButtonDisabled.a = o_colors_radiobutton_disabled;

            float shit = Constraints::RelativeConstraint(0.448f);

            float RadioButtonWidth = Constraints::RelativeConstraint(0.134, "width");
            float RadioButtonHeight = shit;

            if(curr == "modules") FlarialGUI::lerp(width1, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width1, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "settings") FlarialGUI::lerp(width2, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width2, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "scripting") FlarialGUI::lerp(width3, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
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
            static D2D1_COLOR_F tabBgCol4 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;


            tabBgCol.a = o_colors_secondary6;
            tabBgCol2.a = o_colors_secondary6;
            tabBgCol3.a = o_colors_secondary6;
            tabBgCol4.a = o_colors_secondary6;


            //radiobutton of modules

            if(curr != "modules") {
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
                auto& _scrollData = scrollInfo[curr];
                _scrollData.scrollpos = FlarialGUI::scrollpos;
                _scrollData.barscrollpos = FlarialGUI::barscrollpos;

                curr = "modules";
                page.type = "normal";
                FlarialGUI::ResetShit();

                auto& scrollData = scrollInfo[curr];

                FlarialGUI::scrollpos = scrollData.scrollpos;
                FlarialGUI::barscrollpos = scrollData.barscrollpos;
                accumilatedPos = scrollData.scrollpos;
                accumilatedBarPos = scrollData.barscrollpos;
            }


            logoWidth = shit * 0.625f;

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "modules") FlarialGUI::image(IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
                else FlarialGUI::image(IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
            }

            // radiobutton of settings

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            if(curr != "settings") {
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
                                                                                        curr)) {
                auto& _scrollData = scrollInfo[curr];
                _scrollData.scrollpos = FlarialGUI::scrollpos;
                _scrollData.barscrollpos = FlarialGUI::barscrollpos;

                FlarialGUI::TextBoxes[0].isActive = false;
                curr = "settings";

                auto& scrollData = scrollInfo[curr];

                FlarialGUI::scrollpos = scrollData.scrollpos;
                FlarialGUI::barscrollpos = scrollData.barscrollpos;
                accumilatedPos = scrollData.scrollpos;
                accumilatedBarPos = scrollData.barscrollpos;
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "settings") FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
                else FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
            }

            const float h = Constraints::RelativeConstraint(0.42, "height");
            const float allahY = (navy + navigationBarHeight / 2.0f - h / 2.0f);
            ClickGUIElements::SearchBar(0, searchBarString, 12, Constraints::PercentageConstraint(0.022, "right"),
                                        allahY);



            // radiobutton of scripting

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 5.69f, logoWidth) + width1 + width2;
            radioX += radioPushAmount2;
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            if(curr != "scripting") {
                tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{width3, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(3, radioX, radioY,
                                                                                        tabBgCol4,
                                                                                        modTextCol, L"Scripts",
                                                                                        width3,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "scripting",
                                                                                        curr)) {
                auto& _scrollData = scrollInfo[curr];
                _scrollData.scrollpos = FlarialGUI::scrollpos;
                _scrollData.barscrollpos = FlarialGUI::barscrollpos;

                curr = "scripting";
                page.type = "normal";
                FlarialGUI::ResetShit();

                auto& scrollData = scrollInfo[curr];

                FlarialGUI::scrollpos = scrollData.scrollpos;
                FlarialGUI::barscrollpos = scrollData.barscrollpos;
                accumilatedPos = scrollData.scrollpos;
                accumilatedBarPos = scrollData.barscrollpos;
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "scripting") FlarialGUI::image(IDR_SCRIPTING_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
                else FlarialGUI::image(IDR_SCRIPTING_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));

            }

            // radiobutton of editmenu

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 6.45f, logoWidth) + width1 + width2 + width3;
            radioX += radioPushAmount2;
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            if(curr != "editmenu") {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{shit, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(2, radioX, radioY,
                                                                                        tabBgCol3,
                                                                                        modTextCol, L"",
                                                                                        shit,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "editmenu",
                                                                                        curr)) {
                this->active = false;
                FlarialGUI::Notify("To disable this menu press ESC or " +
                                           this->settings.getSettingByName<std::string>("editmenubind")->value);
                editmenu = true;
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if (curr == "editmenu") FlarialGUI::image(IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
                else FlarialGUI::image(IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG", true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));

            }

            /* tab buttons end */

            FlarialGUI::PopSize(); // Pops nav bar
            /* Mod Card Start */

            std::string e = curr;

            if(page.type == "normal" && e == "modules") {
                FlarialGUI::lerp(modcardOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(modcardOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(modcardOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(modcardOffset, -Constraints::RelativeConstraint(2.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }

            if(page.type == "normal" && e == "scripting") {
                FlarialGUI::lerp(scriptingOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(scriptingOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(scriptingOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(scriptingOffset, -Constraints::RelativeConstraint(4.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }


            if(e == "settings" || page.type == "settings") {
                FlarialGUI::lerp(settingsOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(settingsOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(settingsOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(settingsOffset, Constraints::RelativeConstraint(2.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }


            if (page.type == "normal") {


                /* MODCARD RENDER */

                if(modcardOpacity > 0.05f){

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

                    if(ModuleManager::cguiRefresh && ScriptManager::initialized && ModuleManager::initialized) {
                        ModuleManager::updateModulesVector();
                        ModuleManager::cguiRefresh = false;
                    }
                    auto modules = ModuleManager::modulesVector;



                    for (const auto& pModule: modules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 300) <
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
                                ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier, pModule.get(),
                                                          pModule->icon, i, visible);
                                xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier, pModule.get(),
                                                      pModule->icon, i, visible);

                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();

                    D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                    shadowCol.a = modcardOpacity;
                    //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f, center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, center.x + baseWidth, center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
                    }

                /* MODCARD RENDER END */

                /* SETTINGS RENDER */

                if (settingsOpacity > 0.05f) {


                    FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                    float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
                    float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
                    float rectY = Constraints::PercentageConstraint(0.167, "top");
                    float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                    float rectHeight = Constraints::RelativeConstraint(0.85);
                    round = Constraints::RoundingConstraint(38.f, 38.5f);

                    float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                    float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                    D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                    colorThing.a = o_colors_secondary2;

                    D2D1_RECT_F rect = { rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
                    FlarialGUI::PushImClipRect(rect);

                    FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                            anotherRectWidth, anotherRectHeight, round.x, round.x);

                    FlarialGUI::PopImClipRect();

                    D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                    bruv.a = o_colors_secondary1;

                    D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                    textCol.a = o_colors_text;

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectXNoOff + Constraints::SpacingConstraint(0.0085, rectWidth),
                                         rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                         rectHeight);


                    float toggleX = Constraints::PercentageConstraint(0.019, "left");
                    float toggleY = Constraints::PercentageConstraint(0.10, "top");

                    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


                    FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(20.5, scrollviewWidth), 2);
                    FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                              Constraints::RelativeConstraint(1.0, "width"),
                                              Constraints::RelativeConstraint(0.88f, "height"));

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                     rectHeight);

                    static std::string troll = "";
                    if (troll.empty()) troll = Client::settings.getSettingByName<std::string>("currentConfig")->value;

                    if (troll != Client::settings.getSettingByName<std::string>("currentConfig")->value && troll != "") {
                        troll = Client::settings.getSettingByName<std::string>("currentConfig")->value;
                        FlarialGUI::UnsetScrollView();
                        ModuleManager::restartModules = true;
                        FlarialGUI::PopSize();
                        FlarialGUI::PopSize();
                        return;
                    }
                    Module* c = this->ghostMainModule;
                    c->addHeader("Config Manager");
                    c->addDropdown("Selected Config", "", Client::availableConfigs, Client::settings.getSettingByName<std::string>("currentConfig")->value);

                    c->addButton("Add a new config", "", "ADD", [] () {

                        std::random_device rd;
                        std::mt19937 gen(rd());

                        std::uniform_int_distribution<> distrib(1000, 9000);

                        int random_number = distrib(gen);
                        std::string configname = "config-" + std::to_string(random_number);
                        Client::createConfig(configname);
                        Client::settings.getSettingByName<std::string>("currentConfig")->value = configname;
                        FlarialGUI::Notify("Created & loaded: " + configname);
                        ScriptMarketplace::reloadAllConfigs();

                    });
                    c->addButton("Remove selected config", "DELETES YOUR CURRENT CONFIG", "DELETE", [] () {
                        if (Client::settings.getSettingByName<std::string>("currentConfig")->value != "default") {
                            std::string to = Client::settings.getSettingByName<std::string>("currentConfig")->value;
                            Client::settings.getSettingByName<std::string>("currentConfig")->value = "default";
                            Client::deleteConfig(to);
                            ScriptMarketplace::reloadAllConfigs();
                            FlarialGUI::Notify("Deleted " + to);

                        } else {
                            FlarialGUI::Notify("Cannot delete default config.");
                        }
                    });
                    c->addButton("Reload Configs", "Reloads the configs of all modules.", "RELOAD", [] () {
                        ScriptMarketplace::reloadAllConfigs();
                        std::string to = Client::settings.getSettingByName<std::string>("currentConfig")->value;
                        FlarialGUI::Notify("Reloaded " + to);

                    });
                    c->extraPadding();

                    c->addHeader("Keybinds");
                    c->addKeybind("Eject Keybind", "When setting, hold the new bind for 2 seconds", Client::settings.getSettingByName<std::string>("ejectKeybind")->value);

                    c->extraPadding();

                    c->addHeader("Fonts");
                    c->addTextBox("ClickGUI", "", Client::settings.getSettingByName<std::string>("fontname")->value, 48);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("gui_font_scale")->value, 2.f, 0.f, true);
                    c->addTextBox("Modules", "", Client::settings.getSettingByName<std::string>("mod_fontname")->value, 48);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("modules_font_scale")->value, 2.f, 0.f, true);
                    c->addToggle("Override Font Weight", "", Client::settings.getSettingByName<bool>("overrideFontWeight")->value);
                    c->addDropdown("Font Weight", "Bold, Thin, etc.", {"Bold", "Normal", "SemiBold", "ExtraBold", "Medium", "Light", "ExtraLight"}, Client::settings.getSettingByName<std::string>("fontWeight")->value);
                    c->extraPadding();

                    c->addHeader("Rendering");
                    c->addButton("Force Reload Minecraft", "Forces the swapchain to recreate.", "RELOAD", [] () {

                        SwapchainHook::queueReset = true;

                    });


                    c->addButton("Reload Scripts", "", "RELOAD", [&] () {
                        ModuleManager::restartModules = true;
                    });

                    c->addElementText("Following Requires Restart");
                    c->extraPadding();

                    c->addToggle("Better Frames", "RTX Disabled, Restart Required.", Client::settings.getSettingByName<bool>("killdx")->value);
                    c->addToggle("V-SYNC Disabler", "Works on all devices.", Client::settings.getSettingByName<bool>("vsync")->value);
                    if (MC::GPU.contains("Intel")) {
                        c->addToggle("Force Intel DX11", "May help with Better RenderDragon", Client::settings.getSettingByName<bool>("forceIntel")->value);
                    }
                    c->addToggle("Recreate Swapchain At Start", "May help with Better RenderDragon", Client::settings.getSettingByName<bool>("recreateAtStart")->value);
                    c->extraPadding();

                    c->addElementText("Following Does Not Require Restart");
                    c->extraPadding();

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
                    c->addToggle("Snapping Lines", "Y'know, those pink lines in edit mode.", Client::settings.getSettingByName<bool>("snappinglines")->value);
                    c->addToggle("Center Cursor", "Centers your cursor everytime you open your inventory, etc.", Client::settings.getSettingByName<bool>("centreCursor")->value);
                    c->addToggle("Anonymous on API", "Stay anonymous on Flarial's API.", Client::settings.getSettingByName<bool>("anonymousApi")->value);
                    c->addToggle("API Usage", "May disable some features..", Client::settings.getSettingByName<bool>("apiusage")->value);
                    c->addToggle("Promotions", "To donate & join discord. (pls keep on)", Client::settings.getSettingByName<bool>("promotions")->value);
                    c->addToggle("No Flarial Logo", "No Logo next to Nametag </3", Client::settings.getSettingByName<bool>("nologoicon")->value);
                    c->addToggle("No Flarial Chat Icon", "No [FLARIAL] in chat </3", Client::settings.getSettingByName<bool>("nochaticon")->value);

                    c->resetPadding();

                    FlarialGUI::UnsetScrollView();
                    FlarialGUI::PopSize();

                }

                   if(scriptingOpacity > 0.05f){

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


                       auto scriptModules = ScriptManager::getLoadedModules();

                    for (const auto& pModule: scriptModules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 300) <
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
                                ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset, modcenter.y + yModifier, pModule.get(),
                                                          pModule->icon, i, visible, ClickGUI::scriptingOpacity);
                                xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset, modcenter.y + yModifier, pModule.get(),
                                                      pModule->icon, i, visible, ClickGUI::scriptingOpacity);

                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();

                    D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                    shadowCol.a = scriptingOpacity;
                    //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f, center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, center.x + baseWidth, center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
                    }
                /* SETTINGS RENDER END */
                /* Mod Card End */
            }

            std::shared_ptr<Module> settingMod = ModuleManager::getModule(page.module);
            if (!settingMod) {
                settingMod = ScriptManager::getModuleByName(ScriptManager::getLoadedModules(), page.module);
            }
            

            if (settingMod && page.type != "normal" && settingsOpacity > 0.05f) {


                FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
                float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
                float rectY = Constraints::PercentageConstraint(0.167, "top");
                float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                float rectHeight = Constraints::RelativeConstraint(0.85);
                round = Constraints::RoundingConstraint(38.f, 38.5f);

                float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                colorThing.a = o_colors_secondary2;

                D2D1_RECT_F rect = { rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
                FlarialGUI::PushImClipRect(rect);

                FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                        anotherRectWidth, anotherRectHeight, round.x, round.x);

                FlarialGUI::PopImClipRect();

                D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                bruv.a = o_colors_secondary1;

                D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                textCol.a = o_colors_text;

                FlarialGUI::PopSize();

                FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                     rectHeight);


                    FlarialGUI::SetScrollView(
                            rectXNoOff + Constraints::SpacingConstraint(0.0085, rectWidth),
                            rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth, rectHeight);


                settingMod->settingsRender(settingsOffset);

                FlarialGUI::UnsetScrollView();

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


                    textCol.a = settingsOpacity;
                    if (FlarialGUI::RoundedButton(0, spacingX + centered.first + rectX - settingsOffset,
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol, L"Reset",
                                                  buttonWidth, buttonHeight, round.x, round.x)) {
                        auto currentModule = ModuleManager::getModule(page.module);
                        bool wasEnabled = currentModule->isEnabled();
                        currentModule->enabledState = false; // THIS MIGHT BE WITH DELAY !!!
                        currentModule->loadDefaults();
                        FlarialGUI::ResetShit();
                        if(wasEnabled)
                            currentModule->enabledState = true;
                    }


                    if (FlarialGUI::RoundedButton(1, -spacingX + centered.first + rectX + childWidth - buttonWidth - settingsOffset,
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol,
                                                  L"Copy From", buttonWidth, buttonHeight, round.x, round.x)) {

                    }
                }
            }
            FlarialGUI::PopSize(); // Pops base rect
        }

        if(realBlurAmount < 0.5f) setEnabled(false);

        FlarialGUI::displayToolTips();
        FlarialGUI::NotifyHeartbeat();
}
