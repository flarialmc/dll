#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class dvdListener : public Listener {

    Module* module;
    int color = 1;
    float x = 0;
    float y = 0;
    float xv = 1;
    float yv = 1;

    void onRender(RenderEvent& event) override {

        if (module->settings.getSettingByName<bool>("enabled")->value && SDK::clientInstance->getTopScreenName() == "hud_screen") {
            float height = 83 * module->settings.getSettingByName<float>("scale")->value;
            float width = 187 * module->settings.getSettingByName<float>("scale")->value;

            FlarialGUI::Image("\\Flarial\\assets\\dvdlogo-0" + std::to_string(color) + ".png",
                D2D1::RectF(x, y, x + width, y + height));

            x += module->settings.getSettingByName<float>("xveloc")->value * xv;
            y += module->settings.getSettingByName<float>("yveloc")->value * yv;

            if (x >= MC::windowSize.x - width) {
                xv = -1;
                inc();
            }
            if (x < 0) {
                xv = 1;
                inc();
            }
            if (y >= MC::windowSize.y - height) {
                yv = -1;
                inc();
            }
            if (y < 0) {
                yv = 1;
                inc();
            }
        }
    }

    void inc() {
        if (color == 8) color = 1;
        else color++;
    }


public:
    explicit dvdListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
