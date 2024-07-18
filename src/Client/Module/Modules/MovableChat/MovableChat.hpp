#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "MovableChatListener.hpp"

class MovableChat : public Module {

public:

    MovableChat() : Module("Movable Chat", "Makes the Minecraft chat movable.", IDR_MAN_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new MovableChatListener("MovableChat", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("MovableChat");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
        }
        if (settings.getSettingByName<float>("percentageY") == nullptr) {
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);

    }

    void settingsRender() override { }
};