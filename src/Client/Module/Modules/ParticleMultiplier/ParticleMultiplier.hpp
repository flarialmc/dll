#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"


class ParticleMultiplier : public Module {
public:

    ParticleMultiplier(): Module("Particle Multiplier", "Particles go brrrrr",
        IDR_SKULL_PNG, "", false, {"accelerator"}) {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender(RenderEvent& event);
};
