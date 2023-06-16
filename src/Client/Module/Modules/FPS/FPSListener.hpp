#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class FPSListener : public Listener {

    Module* module;

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == this->module->keybind) { this->module->enabled = !this->module->enabled; }
    };

    void onRender(RenderEvent &event) override {

        if(this->module->enabled){

            this->module->NormalRender();

        }

    }

public:
    explicit FPSListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

