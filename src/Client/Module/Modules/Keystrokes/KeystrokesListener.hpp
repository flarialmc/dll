#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include <Windows.h>

class KeystrokesListener : public Listener {

    Module *module;

    void onRender(RenderEvent &event) override {
        if(!module->isEnabled() || SDK::currentScreen != "hud_screen") return;
        this->module->normalRender(7, (std::string &) "");
    }


public:
    explicit KeystrokesListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

