#pragma once

#include <cstdint>
#include <functional>

// LeviLamina

class MobEffect {
public:
    struct FactorCalculationData {
        FactorCalculationData() = delete;

        int paddingDuration;
        float factorStart;
        float factorTarget;
        float factorCurrent;
        std::function<void(FactorCalculationData&, int)> updateFn;
        int effectChangedTimestamp;
        float factorPreviousFrame;
        bool hadEffectLastTick;
        bool hadApplied;
    };

    enum class EffectType : uint32_t {
        Empty = 0,
        Speed = 1,
        Slowness = 2,
        Haste = 3,
        MiningFatigue = 4,
        Strength = 5,
        InstantHealth = 6,
        InstantDamage = 7,
        JumpBoost = 8,
        Nausea = 9,
        Regeneration = 10,
        Resistance = 11,
        FireResistance = 12,
        WaterBreathing = 13,
        Invisibility = 14,
        Blindness = 15,
        NightVision = 16,
        Hunger = 17,
        Weakness = 18,
        Poison = 19,
        Wither = 20,
        HealthBoost = 21,
        Absorption = 22,
        Saturation = 23,
        Levitation = 24,
        FatalPoison = 25,
        ConduitPower = 26,
        SlowFalling = 27,
        BadOmen = 28,
        VillageHero = 29,
        darkness = 30,
    };
};

class MobEffectInstance {
public:
    MobEffect::EffectType id;
    int duration;
    int durationEasy;
    int durationNormal;
    int durationHard;
    int amplifier;
    bool displayOnScreenTextureAnimation;
    bool ambient;
    bool noCounter;
    bool effectVisible;
    MobEffect::FactorCalculationData factorCalculationData;
};

struct MobEffectsComponent
{
    std::vector<MobEffectInstance> effects;
};