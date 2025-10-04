#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


class ParticleMultiplier : public Module {
public:

    ParticleMultiplier(): Module("Particle Multiplier", "Particles go brrrrr",
        IDR_MULTIPLIER_PNG, "", false, {"accelerator"}) {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender(RenderEvent& event);

    void onPacketReceive(PacketEvent& event);

    void onAttack(AttackEvent& event);
};
