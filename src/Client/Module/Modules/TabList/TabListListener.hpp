#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class TabListListener : public Listener {

    Module* module;

public:
    explicit TabListListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};