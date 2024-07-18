#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CPSListener.hpp"


class CPSCounter : public Module {

public:


    CPSCounter() : Module("CPS", "Counts your Clicks per second.", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
        EventHandler::registerListener(new CPSListener("CPS", this));
    }

    void onEnable() override {
        Module::onEnable();
    }

    void onDisable() override {
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("rightcps") == nullptr) {
            settings.addSetting("rightcps", false);
        }

        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "CPS: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override { }
};