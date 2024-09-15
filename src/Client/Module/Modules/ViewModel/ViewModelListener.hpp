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

class ViewModelListener : public Listener {

    Module *module;

    void onGetFOV(FOVEvent &event) override {
        auto fov = event.getFOV();
        if (fov != 70) return;

        event.setFOV(module->settings.getSettingByName<float>("itemfov")->value);
    }
public:
    explicit ViewModelListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
