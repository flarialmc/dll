#pragma once

#include "HueListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class HueChanger : public Module {

public:

    HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.",
                          IDR_FULLBRIGHT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerPriorityListener(new HueListener("Hue", this));
        Module::onEnable();
    }


    void onDisable() override {
        EventHandler::unregisterListener("Hue");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.5f);
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
        this->addSlider("Saturation Intensity", "", this->settings.getSettingByName<float>("intensity")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};