#pragma once

#include "../Module.hpp"
#include "PatarHDListener.hpp"
#include "../../../Events/EventHandler.hpp"


class PatarHD : public Module {

public:

    PatarHD() : Module("PatarHD", "Who is this now?", "\\Flarial\\assets\\skull.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new PatarHDListener("PatarHDListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {


        if(settings.getSettingByName<bool>("enabled") == nullptr)
            settings.addSetting("enabled", false);

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {
    }
};