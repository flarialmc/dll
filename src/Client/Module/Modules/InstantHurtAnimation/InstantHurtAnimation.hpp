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
        if (settings.getSettingByName<bool>("onlyWithArmor") == nullptr) settings.addSetting("onlyWithArmor", true);
        if (settings.getSettingByName<bool>("tryToExcludeTeam") == nullptr) settings.addSetting("tryToExcludeTeam", true);
    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Try to exclude team", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "tryToExcludeTeam")->value))
            this->settings.getSettingByName<bool>("tryToExcludeTeam")->value = !this->settings.getSettingByName<bool>(
                    "tryToExcludeTeam")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Only with armor", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "onlyWithArmor")->value))
            this->settings.getSettingByName<bool>("onlyWithArmor")->value = !this->settings.getSettingByName<bool>(
                    "onlyWithArmor")->value;
    }
};
