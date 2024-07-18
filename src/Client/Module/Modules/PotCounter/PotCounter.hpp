#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "PotListener.hpp"

class PotCounter : public Module {

public:

    PotCounter() : Module("Pot Counter", "Counts how much potions are\nin your inventory.",
                          IDR_POTION_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new PotListener("Pot", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Pot");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} Pots");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override { }
};