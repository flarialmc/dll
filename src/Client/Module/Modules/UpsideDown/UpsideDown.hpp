#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class UpsideDown : public Module {

public:
    UpsideDown();;

    void onEnable() override;

    void onDisable() override;

    static void onGetFOV(FOVEvent &event);

    void defaultConfig() override;
};

