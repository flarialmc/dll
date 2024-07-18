#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "MemoryListener.hpp"


class MEM : public Module {

public:


    MEM() : Module("Memory", "Shows your current system RAM usage.", IDR_MEMORY_PNG, "") {

        Module::setup();
    };

    void onEnable() override {

        EventHandler::registerListener(new MemoryListener("Memory", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Memory");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");
    }

    void settingsRender() override { }
};

