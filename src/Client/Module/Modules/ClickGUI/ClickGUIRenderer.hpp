#pragma once
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

class ClickGUIRenderer : public Listener {

    Module* module;

    void onRender(RenderEvent &event) override {


        if(module->enabled && SwapchainHook::init) {

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.9);
            float baseHeight = Constraints::RelativeConstraint(0.64);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth, baseHeight, "r", 1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(50, 50);

            FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), baseWidth, baseHeight, round.x, round.x);
            FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

            /* Base Rectangle End */


            /* Nav Bar Start */

            float navigationBarWidth = Constraints::RelativeConstraint(1.384f);
            float navigationBarHeight = Constraints::RelativeConstraint(0.14f);
            float navx = Constraints::PercentageConstraint(0.008f, "left");
            float navy = Constraints::PercentageConstraint(0.009f, "top");
            round = Constraints::RoundingConstraint(50, 50);

            FlarialGUI::RoundedRect(navx, navy, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), navigationBarWidth, navigationBarHeight, round.x ,round.x);


            FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

            /* Nav Bar End */

            /* Logo Start */


            float logoWidth = Constraints::RelativeConstraint(1.21);

            float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidth);
            float logoY = (navy + navigationBarHeight / 2.0f - logoWidth / 2.0f);

            FlarialGUI::Image("\\Flarial\\assets\\logo.png", D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth), logoWidth , logoWidth);

            FlarialGUI::PopSize(); // Pops nav bar

            /* Logo End */

            /* Mod Card Start */



            float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
            float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

            Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.55, -0.52);


            float scrollWidth = Constraints::RelativeConstraint(1.12);
            float scrollHeight = Constraints::RelativeConstraint(0.84);
            Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0, 1);

            FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

            float xModifier = 0.0f;
            float yModifier = 0.0f;

            for (size_t i = 0; i < ModuleManager::modules.size(); i++) {
                Module* real = ModuleManager::modules[i];
                ClickGUIElements::ModCard(modcenter.x + xModifier, modcenter.y + yModifier, real, real->icon);

                xModifier += Constraints::SpacingConstraint(1.09, modWidth);
                if ((i + 1) % 3 == 0) {
                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                    xModifier = 0.0f;
                }
            }

            FlarialGUI::UnsetScrollView();
            FlarialGUI::ScrollBar(120, 120, 10, 160, 2);

            //ClickGUIElements::ModCard(modcenter.x, modcenter.y, ModuleManager::modules.front(), "");

            FlarialGUI::PopSize(); // Pops base rect

            /* Mod Card End */

        }

    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind && static_cast<ActionType>(event.GetAction()) == ActionType::PRESSED) module->enabled = !module->enabled;
    }

public:
    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
    }
};
