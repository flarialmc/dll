#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class PingListener : public Listener { ;
    Module *module;

    void onRender(RenderEvent &event) override {
        auto pingStr = std::to_string(SDK::getServerPing());
        this->module->normalRender(11, pingStr);
    }

public:
    explicit PingListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

