#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ForceCoordsListener.hpp"
#include "../../../Client.hpp"

class ForceCoords : public Module {

public:


    ForceCoords() : Module("Coordinates", "Shows your ingame position. (XYZ)", IDR_COORDINATES_PNG,
                           "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new ForceCoordsListener("ForceCoords", this));
        Module::onEnable();
    }

    void onDisable() override {
        ForceCoordsListener::unpatch();

        EventHandler::unregisterListener("ForceCoords");

        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");

        if (settings.getSettingByName<bool>("MojangStyle") == nullptr) settings.addSetting("MojangStyle", false);

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);

    }

    void settingsRender() override { }
};

