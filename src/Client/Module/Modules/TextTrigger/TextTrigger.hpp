#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TextTriggerListener.hpp"


class TextTrigger : public Module {

public:


    TextTrigger() : Module("Text Trigger", "Automatically sends a message when you\nget a sertain text.",
                           R"(\Flarial\assets\like.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new TextTriggerListener("TextTrigger", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("TextTrigger");
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

