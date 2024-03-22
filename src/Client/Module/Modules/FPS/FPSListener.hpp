#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class FPSListener : public Listener {

    Module *module;

    void onRender(RenderEvent &event) override {

        if (module->isEnabled()) {

            int fps = (int)round((MC::fps *
                    round(module->settings.getSettingByName<float>(
                            "fpsSpoofer")->value)));

            auto fpsStr = std::to_string(fps);

            this->module->normalRender(0, fpsStr);

        }

    }

public:
    explicit FPSListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

