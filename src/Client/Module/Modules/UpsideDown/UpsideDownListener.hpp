#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include "../../Manager.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"

class UpsideDownListener : public Listener {

    Module *module;

    void onGetFOV(FOVEvent &event) override {
        auto fov = event.getFOV();
        fov = 360 - fov;
        event.setFOV(fov);
    }

public:

    explicit UpsideDownListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
