#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class NickListener : public Listener {
    Module* module;
public:
    explicit NickListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
