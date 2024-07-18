#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ArrowListener.hpp"

class ArrowCounter : public Module {

public:

    ArrowCounter() : Module("Arrow Counter", "Counts how many arrows you have\nin your inventory.",
                            IDR_ARROW_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new ArrowListener("Arrow", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Arrow");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} Arrows");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override { }
};