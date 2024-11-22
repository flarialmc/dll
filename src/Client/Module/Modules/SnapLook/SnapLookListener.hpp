#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <windows.h>

class SnapListener : public Listener {

    Module *module;

    // TODO: make it togglable
    void onKey(KeyEvent &event) override {

        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key))
            module->active = !module->active;


        if (!module->isKeybind(event.keys)) module->active = false;

    };

    void onGetViewPerspective(PerspectiveEvent &event) override {
        if (module->active)
            event.setPerspective(Perspective::ThirdPersonFront);
    }

public:
    explicit SnapListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
