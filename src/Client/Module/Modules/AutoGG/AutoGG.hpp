#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoGGListener.hpp"


class AutoGG : public Module {

public:


    AutoGG() : Module("Auto GG", "oAnshul==bari", "\\Flarial\\assets\\like.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"GG");

        EventHandler::registerListener(new AutoGGListener("AutoGG", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("AutoGG");

        Module::onDisable();

    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 16, toggleX, toggleY);

    }
};

