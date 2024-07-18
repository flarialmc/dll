#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class DeepfryListener : public Listener {

    Module *module;

    void onRender(RenderEvent &event) override {

        if (module->isEnabled()) {
            //if (module->settings.getSettingByName<bool>("paint")->value) FlarialGUI::ApplyPaintEffect(10.0f);
            //else FlarialGUI::ApplyCombinedDeepFry();

        }

    }

public:
    explicit DeepfryListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

