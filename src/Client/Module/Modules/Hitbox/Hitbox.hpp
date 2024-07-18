#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "HitboxListener.hpp"

class Hitbox : public Module {

public:

    Hitbox() : Module("Hitbox", "Displays hitboxes of entities", IDR_BLOCK_PNG, "") {

        Module::setup();

    };

    // TODO: add options to see all entities

    void onEnable() override {
        EventHandler::registerListener(new HitboxListener("Hitbox", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Hitbox");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);
        if (settings.getSettingByName<float>("thickness") == nullptr) settings.addSetting("thickness", 1.1f);
        if (settings.getSettingByName<bool>("staticThickness") == nullptr) settings.addSetting("staticThickness", false);
        if (settings.getSettingByName<bool>("outline") == nullptr) settings.addSetting("outline", false);
    }

    void settingsRender() override { }

};
