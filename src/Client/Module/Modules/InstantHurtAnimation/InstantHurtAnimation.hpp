#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "InstantHurtAnimationListener.hpp"


class InstantHurtAnimation : public Module {

public:

    InstantHurtAnimation() : Module("Instant Hurt Animation", "Hurt animation becomes ping independent, helps time hits.",
                          IDR_COMBO_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new InstantHurtAnimationListener("InstantHurtAnimation", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("InstantHurtAnimation");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<bool>("tryToExcludeTeam") == nullptr) settings.addSetting("tryToExcludeTeam", false);

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
        this->addToggle("Try to Exclude Team", "E.g. your team in Bedwars", this->settings.getSettingByName<bool>("tryToExcludeTeam")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};
