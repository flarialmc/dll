#pragma once

#include "../Module.hpp"

class DepthOfField : public Module {
public:
    DepthOfField() : Module("Depth Of Field", "",
        IDR_DOF_PNG, "", false, {"dof"}) {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender(RenderUnderUIEvent& event);
};
