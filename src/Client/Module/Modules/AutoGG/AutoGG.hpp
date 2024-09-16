#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoGGListener.hpp"


class AutoGG : public Module {

public:


    AutoGG() : Module("Auto GG", "Automatically sends a message when you\nwin a game. (Doesn't work everywhere)",
                      IDR_LIKE_PNG, "") {

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


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addTextBox("Text", "The text to send.", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};

