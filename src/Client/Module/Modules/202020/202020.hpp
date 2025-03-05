#pragma once

#include "../Module.hpp"

class 202020 : public Module {
public:
    auto now;
    auto last;
    auto elapsed;

    202020() : Module("20 20 20", "Reminds you to look at \nsomething 20ft away every 20m",
                          IDR_FULLBRIGHT_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        now = std::chrono::steady_clock::now();
        last = now;
        Listen(this, TickEvent, &202020::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &202020::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {}

    void settingsRender(float settingsOffset) override {}
    
    void onTick(TickEvent &event) {
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last);
        if (elapsed >= std::chrono::seconds(1200)) { 
            FlarialGUI::Notify("Look at something 20 feet away for 20 Seconds!");
        }
    }
};