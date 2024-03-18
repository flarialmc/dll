#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AnimationsListener.hpp"


class Animations : public Module {

public:

    Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
                          R"(\Flarial\assets\Animations.png)", "") {
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

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"HotBar", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("HotBar "),
                                           toggleY, this->settings.getSettingByName<float>("hotbarSpeed")->value, 50.0f,
                                           0, false);

        this->settings.getSettingByName<float>("hotbarSpeed")->value = percent;

    }
};
