#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoRQListener.hpp"


class AutoRQ : public Module {

public:


    AutoRQ() : Module("Auto RE Q", "Automatically requeues into a game (Hive)",
                      R"(\Flarial\assets\re-q.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new AutoRQListener("AutoRQ", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("AutoRQ");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string)"");
    }

    void settingsRender() override {

    }
};

