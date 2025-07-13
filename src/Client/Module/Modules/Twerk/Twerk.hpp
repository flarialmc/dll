#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include <chrono>

class Twerk : public Module {

private:
    bool ini = false;
    std::chrono::steady_clock::time_point lastShift;
    int shiftCount = 0;

public:

    Twerk(): Module("Twerk", "yeah baby throw it back",
        IDR_TWERK_PNG, "") {
    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onTick(TickEvent& event);
};

