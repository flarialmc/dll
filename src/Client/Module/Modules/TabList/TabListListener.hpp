#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>
#include <iostream>

class TabListListener : public Listener {

    Module* module;

    void onRender(RenderEvent &event) override {
        std::string text;
        this->module->normalRender(20, text);
    }

    void onKey(KeyEvent &event) override {
        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
            module->active = !module->active;
        }

        if (!module->isKeybind(event.keys)) module->active = false;

    };



public:
    explicit TabListListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};