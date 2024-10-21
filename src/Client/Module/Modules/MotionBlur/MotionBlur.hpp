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
        if(SwapchainHook::queue) FlarialGUI::Notify("Please turn on Better Frames in Settings!");
        else {
            EventHandler::registerPriorityListener(new MotionBlurListener("MotionBlurListener", this));
            Module::onEnable();
        }
    }

    void onDisable() override {
        EventHandler::unregisterListener("MotionBlurListener");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.88f);
        if (settings.getSettingByName<float>("intensity2") == nullptr) settings.addSetting("intensity2", 6.0f);
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("Bleed Factor", "The scale at which previous frames bleed into the current one.", this->settings.getSettingByName<float>("intensity")->value, 1.0f, 0.f, true);
        this->addSlider("Intensity", "Amount of previous frames to render.", this->settings.getSettingByName<float>("intensity2")->value, 30, 0, true);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};