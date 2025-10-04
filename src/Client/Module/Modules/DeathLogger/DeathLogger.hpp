#pragma once

#include "../Module.hpp"

#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"

class DeathLogger : public Module {

private:
    bool death = false;
    bool waypointed = false;
    bool printed = false;
    Vec3<float> deathPos;

public:

    DeathLogger() : Module("Death Logger", "Logs your coordinates in chat on death.",
        IDR_DEATHLOGGER_PNG, "", false, {"death waypoints", "waypoint"}) {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    static std::string getRawDimensionName();

    void onTick(TickEvent& event);

};
