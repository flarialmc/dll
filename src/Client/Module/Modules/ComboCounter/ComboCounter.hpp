#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include  "ComboListener.hpp"

class ComboCounter : public Module {

public:

    ComboCounter() : Module("Combo", "Keeps track of consecutive hits.", IDR_COMBO_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new ComboListener("Combo", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Combo");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "Combo: {value}");
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override { }
};