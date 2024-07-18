#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SprintListener.hpp"


class Sprint : public Module {

public:


    Sprint() : Module("Toggle Sprint", "Automatically sprints for you!!!", IDR_AUTO_SPRINT_PNG, "SHIFT") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new SprintListener("Sprint", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Sprint");
        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");

        if (settings.getSettingByName<bool>("status") == nullptr) settings.addSetting("status", false);

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);

        if (settings.getSettingByName<bool>("always") == nullptr) {
            settings.addSetting("always", false);
        }
    }

    void settingsRender() override { }
};

