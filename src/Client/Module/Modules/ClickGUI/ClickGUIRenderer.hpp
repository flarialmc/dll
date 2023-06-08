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

class ClickGUIRenderer : public Listener {

    bool enabled = false;
    Module* module;

    void onRender(RenderEvent &event) override {


        if(enabled) {

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.78);
            float baseHeight = Constraints::RelativeConstraint(0.7);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth, baseHeight, "r", 1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(50, 50);

            FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), baseWidth, baseHeight, round.x, round.x);
            FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

             /* Base Rectangle End */


             /* Nav Bar Start */

            float navigationBarWidth = Constraints::RelativeConstraint(1.097f);
            float navigationBarHeight = Constraints::RelativeConstraint(0.118f);
            float navx = Constraints::PercentageConstraint(0.008f, "left");
            float navy = Constraints::PercentageConstraint(0.007f, "top");
            round = Constraints::RoundingConstraint(45, 45);

            FlarialGUI::RoundedRect(navx, navy, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), navigationBarWidth, navigationBarHeight, round.x ,round.x);


            FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

             /* Nav Bar End */

            /* Logo Start */


            float logoWidth = 69;
            float logoHeight = 69;

            float logoX = navx;
            float logoY = navy + navigationBarHeight / 2.0f - logoHeight / 2.0f;

            FlarialGUI::Image("\\Flarial\\assets\\logo.png", D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoHeight), logoWidth , logoHeight);

            FlarialGUI::PopSize(); // Pops nav bar

            /* Logo End */

            FlarialGUI::PopSize(); // Pops base rect

        }

    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind && event.GetAction() == ActionType::PRESSED) enabled = !enabled;
    }

public:
    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
    }
};
