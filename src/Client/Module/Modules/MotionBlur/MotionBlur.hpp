#pragma once

#include "../Module.hpp"
#include "MotionBlurListener.hpp"
#include "../../../Events/EventHandler.hpp"


class MotionBlur : public Module {

public:

    MotionBlur() : Module("MotionBlur", "chike wi!", "\\Flarial\\assets\\nametag.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new MotionBlurListener("MotionBlurListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {



    }
};