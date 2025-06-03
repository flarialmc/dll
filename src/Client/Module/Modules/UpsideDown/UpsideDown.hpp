#pragma once

#include "../Module.hpp"
#include "Events/Game/FOVEvent.hpp"

class UpsideDown : public Module {

public:
    UpsideDown();;

    void onEnable() override;

    void onDisable() override;

    void onGetFOV(FOVEvent &event);

    void defaultConfig() override;
};

