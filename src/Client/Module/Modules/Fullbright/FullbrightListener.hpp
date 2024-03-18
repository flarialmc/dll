#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <windows.h>

class FullbrightListener : public Listener {

    Module *module;
public:
    // TODO: Make it changable
    void onGetGamma(GammaEvent &event) override {
        event.setGamma(module->settings.getSettingByName<float>("gamma")->value);
    };

    explicit FullbrightListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
