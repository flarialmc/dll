#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:
    Deepfry() : Module("Deepfry", "Theres only one way to find out.", IDR_FRYING_PAN_PNG, "") {

        Module::setup();
    };

    void onEnable() override {

        EventHandler::registerPriorityListener(new DeepfryListener("Deepfry", this));

        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("Deepfry");

        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);
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
        this->addToggle("Paint Effect", "", this->settings.getSettingByName<bool>("paint")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();


    }
};