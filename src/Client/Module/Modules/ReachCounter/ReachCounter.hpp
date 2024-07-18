#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ReachListener.hpp"

class ReachCounter : public Module {

public:

    ReachCounter() : Module("Reach Counter", "Displays your last hit range in blocks.", IDR_REACH_PNG,
                            "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new ReachListener("Reach", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("Reach");
        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "Reach: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override { }
};