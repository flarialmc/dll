#pragma once

#include "../Module.hpp"
#include "MotionBlurListener.hpp"
#include "../../../Events/EventHandler.hpp"


class MotionBlur : public Module {

public:

    MotionBlur() : Module("Motion Blur",
                          "Make fast movements appear smoother and more realistic by\nblurring the image slightly in the direction of motion.",
                          IDR_BLUR_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerPriorityListener(new MotionBlurListener("MotionBlurListener", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("MotionBlurListener");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.88f);
        if (settings.getSettingByName<float>("intensity2") == nullptr) settings.addSetting("intensity2", 6.0f);
    }

    void settingsRender() override { }
};