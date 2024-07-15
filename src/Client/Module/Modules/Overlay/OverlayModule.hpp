#pragma once

#include "../Module.hpp"
#include "OverlayListener.hpp"
#include "../../../Events/EventHandler.hpp"


class OverlayModule : public Module {

public:

    OverlayModule() : Module("Overlay",
                          "xD",
                          R"(\Flarial\assets\icognito.png)", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new OverlayListener("OverlayListener", this));

        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("OverlayListener");
        Module::onDisable();
    }

    void defaultConfig() override {

    }

    void settingsRender() override {
        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("nick")->value, 16, toggleX, toggleY);
    }
};