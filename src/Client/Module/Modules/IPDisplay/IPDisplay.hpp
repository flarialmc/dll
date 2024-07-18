#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "IPDisplayListener.hpp"
#include "../../../Client.hpp"


class IPDisplay : public Module {

public:


    IPDisplay() : Module("IP Display", "Displays the current server IP you're playing on.",
                         IDR_SERVER_IP_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new IPDisplayListener("IPDisplay", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("IPDisplay");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
    }

    void settingsRender() override { }
};

