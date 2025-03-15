#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class LowHealthIndicator : public Module {
    int health = 20.f;
public:
    LowHealthIndicator() : Module("Low Health", "Warns you when you are at low health.", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Module::onEnable();
    }

    void onDisable() override {
        Module::onDisable();
    }

    void onSetupAndRender(SetupAndRenderEvent& event) {
        if (SDK::clientInstance) if (SDK::clientInstance->getLocalPlayer())
        health = SDK::clientInstance->getLocalPlayer()->getHealth();
    }

    void onRender(RenderEvent& event) {
        if (this->isEnabled() && this->health < 7) {
            D2D1_COLOR_F color2 = D2D1::ColorF(D2D1::ColorF::Red);
            FlarialGUI::RoundedRect(0, 0, color2, MC::windowSize.x, MC::windowSize.y, 0, 0);
        }
    }
};
