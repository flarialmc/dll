#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "HurtColorListener.hpp"


class HurtColor : public Module {

public:

    HurtColor() : Module("Hurt Color", "Change the color when you hit entities.", IDR_HURT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new HurtColorListener("HurtColor", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("HurtColor");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.65);
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Colors");
        this->addColorPicker("Hurt Color", "", settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};