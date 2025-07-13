#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"

class DeathLogger : public Module {

private:
    bool death = false;
    bool printed = false;
    Vec3<float> deathPos;

public:

    DeathLogger() : Module("Death Logger", "Logs your coordinates in chat on death.\nMAY BREAK WITH CUSTOM UI TEXTURE PACKS",
        IDR_SKULL_PNG, "") {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    static std::string getRawDimensionName();

    void onTick(TickEvent& event);

};
