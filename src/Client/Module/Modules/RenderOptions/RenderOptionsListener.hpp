#pragma once

#include "../../../Events/Listener.hpp"
#include "../Module.hpp"

class RenderOptionsListener : public Listener {

    Module *module;

    void onGetGamma(GammaEvent &event) override { // TODO: Make options loaded event?
        module->onEnable();
        EventHandler::unregisterListener("RenderOptions");
    }

public:
    explicit RenderOptionsListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

