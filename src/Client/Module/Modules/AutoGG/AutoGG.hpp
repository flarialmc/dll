#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoGGListener.hpp"


class AutoGG : public Module {

public:


    AutoGG() : Module("Auto GG", "oAnshul==bari", "\\Flarial\\assets\\time.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new AutoGGListener("AutoGG", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("AutoGG");

        Module::onDisable();

    }

    void SettingsRender() override {
    }
};

