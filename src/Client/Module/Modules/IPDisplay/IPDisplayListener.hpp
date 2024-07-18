#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Hook/Hooks/Game/RaknetTick.hpp"
#include <Windows.h>

class IPDisplayListener : public Listener {

    Module *module;

    void onRender(RenderEvent &event) override {
        std::string IPStr = SDK::getServerIP();
        this->module->normalRender(10, IPStr);
    }

public:
    explicit IPDisplayListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

