#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SnapLookListener.hpp"

class SnapLook : public Module {

public:

    SnapLook() : Module("SnapLook", "Quickly look behind you.", IDR_EYE_PNG, "V") {

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


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Keybind", "", getKeybind());

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};