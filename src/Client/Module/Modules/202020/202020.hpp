#pragma once

#include "../Module.hpp"

class Module202020 : public Module {
public:
    std::chrono::steady_clock::time_point now;
    std::chrono::steady_clock::time_point last;
    std::chrono::seconds elapsed;

    Module202020() : Module("Meds", "Implements the medical 202020 rule\ninto minecraft.", IDR_TIME_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        now = std::chrono::steady_clock::now();
        last = now;
        Listen(this, TickEvent, &Module202020::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &Module202020::onTick)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig(); }

    void settingsRender(float settingsOffset) override {}
    
    void onTick(TickEvent &event) {
        now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last);

        if (elapsed >= std::chrono::minutes(20)) {
            FlarialGUI::Notify("Look at something 20 feet away for 20 seconds!");
            last = now;
        }
    }
};