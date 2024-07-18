#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FullbrightListener.hpp"


class Fullbright : public Module {

public:

    Fullbright() : Module("Fullbright",
                          "No need for torches!\nProvides consistent and constant illumination.\nEffectively removing darkness and shadows.",
                          IDR_FULLBRIGHT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new FullbrightListener("Fullbright", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Fullbright");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("gamma") == nullptr)
            settings.addSetting("gamma", 25.0f);
    }

    void settingsRender() override { }
};