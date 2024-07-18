#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "PingListener.hpp"

class PingCounter : public Module {

public:

    PingCounter() : Module("Ping Counter", "Displays your current latency to the server.",
                           IDR_PING_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new PingListener("Ping", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("Ping");
        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} Ping");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);

    }

    void settingsRender() override { }
};