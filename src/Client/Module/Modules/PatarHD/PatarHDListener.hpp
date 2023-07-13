#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class PatarHDListener : public Listener {

    Module* module;

    void onRender(RenderEvent& event) override {

        if(module->settings.getSettingByName<bool>("enabled")->value && SDK::clientInstance->getTopScreenName() == "hud_screen"){
            float s = Constraints::RelativeConstraint(0.35, "height", true);
            Vec2<float> center = Constraints::CenterConstraint(s, s);

            FlarialGUI::Image("\\Flarial\\assets\\patar.jpg",
                              D2D1::RectF(center.x, center.y, center.x + s, center.y + s));
        }

    }


public:
    explicit PatarHDListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
