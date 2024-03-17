#pragma once

#include "../Module.hpp"
#include "ThirdPersonListener.hpp"
#include "../../../Events/EventHandler.hpp"


class ThirdPerson : public Module {

public:

    ThirdPerson() : Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.",
                           R"(\Flarial\assets\nametag.png)", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new ThirdPersonListener("ThirdPersonListener", this));
        Module::onEnable();

    }

    void onDisable() override {
        ThirdPersonListener::unpatch();
        EventHandler::unregisterListener("ThirdPersonListener");
        Module::onDisable();
    }
};