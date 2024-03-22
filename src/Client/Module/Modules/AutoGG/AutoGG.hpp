#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoGGListener.hpp"


class AutoGG : public Module {

public:


    AutoGG() : Module("Auto GG", "Automatically sends a message when you\nwin a game. (Doesn't work everywhere)",
                      R"(\Flarial\assets\like.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new AutoGGListener("AutoGG", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("AutoGG");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string) "GG");
    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 50, toggleX, toggleY);

    }
};

