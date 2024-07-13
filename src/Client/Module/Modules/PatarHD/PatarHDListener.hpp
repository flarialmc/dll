#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class PatarHDListener : public Listener {

    Module *module;
    float x = 0;
    float y = 0;
    float xv = 1;
    float yv = 1;

    void onRender(RenderEvent &event) override {

        if (module->isEnabled() &&
            ClientInstance::getTopScreenName() == "hud_screen") {
            float s = Constraints::RelativeConstraint(0.35, "height", true) *
                      module->settings.getSettingByName<float>("scale")->value;
            if (module->settings.getSettingByName<bool>("dvdmode")->value) {
                FlarialGUI::image(IDR_PATAR_JPG,
                                  D2D1::RectF(x, y, x + s, y + s), "JPG");

                x += module->settings.getSettingByName<float>("xveloc")->value * xv;
                y += module->settings.getSettingByName<float>("yveloc")->value * yv;

                if (x >= MC::windowSize.x - s) xv = -1;
                if (x < 0) xv = 1;
                if (y >= MC::windowSize.y - s) yv = -1;
                if (y < 0) yv = 1;
            } else {
                Vec2<float> center = Constraints::CenterConstraint(s, s);
                FlarialGUI::image(IDR_PATAR_JPG,
                                  D2D1::RectF(center.x, center.y, center.x + s, center.y + s));
            }
        }

    }


public:
    explicit PatarHDListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
