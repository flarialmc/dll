#pragma once

#include "../Module.hpp"
#include "Events/Game/FOVEvent.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"

class UpsideDown : public Module {

public:

    UpsideDown(): Module("Upside Down", "No need to flip your monitor!!",
        IDR_UPSIDE_DOWN_PNG, "", false, {"troll"}) {

    }

    void onEnable() override;

    void onDisable() override;

    void onGetFOV(FOVEvent &event);

    void defaultConfig() override;
};

