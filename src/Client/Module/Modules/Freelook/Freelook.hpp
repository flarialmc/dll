#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "LookListener.hpp"

class FreeLook : public Module {

public:

    FreeLook() : Module("FreeLook",
                        "Freely move your camera in 3rd person mode\nwhile keeping the player rotation the same.",
                        IDR_FREELOOK_PNG, "F") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new LookListener("Look", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Look");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("toggle") == nullptr) settings.addSetting("toggle", false);

        if (settings.getSettingByName<std::string>("mode") == nullptr)
            settings.addSetting("mode", (std::string) "3rd Person back");
    }



    void settingsRender() override { }
};