#pragma once

#include "../Module.hpp"
#include "dvdListener.hpp"
#include "../../../Events/EventHandler.hpp"


class DVD : public Module {

public:

    DVD() : Module("DVD Screen", "Overlays the DVD Screensaver", IDR_SKULL_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new dvdListener("dvdListener", this));

        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("dvdListener");

        Module::onDisable();

    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("xveloc") == nullptr)
            settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr)
            settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 1.0f);

    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Sizes");
        this->addSlider("Scale", "", this->settings.getSettingByName<float>("scale")->value);
        this->addSlider("X Velocity", "", this->settings.getSettingByName<float>("xveloc")->value);
        this->addSlider("Y Velocity", "", this->settings.getSettingByName<float>("yveloc")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};