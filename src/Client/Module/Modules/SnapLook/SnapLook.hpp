#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SnapLookListener.hpp"

class SnapLook : public Module {

public:

    SnapLook() : Module("SnapLook", "Quickly look behind you.", R"(\Flarial\assets\eye.png)", "V") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new SnapListener("Snap", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Snap");
        Module::onDisable();
    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, getKeybind());
    }
};