#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AnimationsListener.hpp"


class Animations : public Module {

public:

    Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
                          IDR_ANIMATIONS_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new AnimationsListener("Animations", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Animations");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("hotbarSpeed") == nullptr) settings.addSetting("hotbarSpeed", 7.0f);

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
        this->extraPadding();

        this->addSlider("Hotbar", "", this->settings.getSettingByName<float>("hotbarSpeed")->value);

        FlarialGUI::UnsetScrollView();
    }
};
