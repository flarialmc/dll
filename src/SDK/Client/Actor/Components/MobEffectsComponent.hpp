#pragma once

#include <cstdint>
#include <map>
#include <SDK/Client/Render/TexturePtr.hpp>
#include <SDK/Client/Actor/EntityContext.hpp>
#include <Utils/VersionUtils.hpp>

#include "Assets/Assets.hpp"
#include "GUI/Engine/Engine.hpp"

// LeviLamina

class MobEffect {
public:
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
        Darkness = 30,
        TrialOmen = 31,
        WindCharged = 32,
        Weaving = 33,
        Oozing = 34,
        Infested = 35,
        RaidOmen = 36
    };

    static std::pair<bool, int> hasTexture(EffectType type) {
        switch (type) {
            case EffectType::InstantHealth: return std::pair{false, IDR_INSTANTHEALTH_PNG};
            case EffectType::InstantDamage: return std::pair{false, IDR_INSTANTDAMAGE_PNG};
            case EffectType::Saturation: return std::pair{false, IDR_SATURATION_PNG};
            default: return std::pair{true, IDR_AUTORQ_PNG};
        }
    }

    static std::string effectTypeToPath(EffectType type) {
        static const std::map<EffectType, std::string> effectToTexture = {
            {EffectType::Speed, "speed_effect"},
            {EffectType::Slowness, "slowness_effect"},
            {EffectType::Haste, "haste_effect"},
            {EffectType::MiningFatigue, "mining_fatigue_effect"},
            {EffectType::Strength, "strength_effect"},
            {EffectType::InstantHealth, "instant_health_effect"},
            {EffectType::InstantDamage, "instant_damage_effect"},
            {EffectType::JumpBoost, "jump_boost_effect"},
            {EffectType::Nausea, "nausea_effect"},
            {EffectType::Regeneration, "regeneration_effect"},
            {EffectType::Resistance, "resistance_effect"},
            {EffectType::FireResistance, "fire_resistance_effect"},
            {EffectType::WaterBreathing, "water_breathing_effect"},
            {EffectType::Invisibility, "invisibility_effect"},
            {EffectType::Blindness, "blindness_effect"},
            {EffectType::NightVision, "night_vision_effect"},
            {EffectType::Hunger, "hunger_effect"},
            {EffectType::Weakness, "weakness_effect"},
            {EffectType::Poison, "poison_effect"},
            {EffectType::Wither, "wither_effect"},
            {EffectType::HealthBoost, "health_boost_effect"},
            {EffectType::Absorption, "absorption_effect"},
            {EffectType::Saturation, "saturation_effect"},
            {EffectType::Levitation, "levitation_effect"},
            {EffectType::FatalPoison, "fatal_poison_effect"},
            {EffectType::ConduitPower, "conduit_power_effect"},
            {EffectType::SlowFalling, "slow_falling_effect"},
            {EffectType::BadOmen, "bad_omen_effect"},
            {EffectType::VillageHero, "village_hero_effect"},
            {EffectType::Darkness, "darkness_effect"},
            {EffectType::TrialOmen, "trial_omen_effect"},
            {EffectType::WindCharged, "wind_charged_effect"},
            {EffectType::Weaving, "weaving_effect"},
            {EffectType::Oozing, "oozing_effect"},
            {EffectType::Infested, "infested_effect"},
            {EffectType::RaidOmen, "raid_omen_effect"},
        };

        auto it = effectToTexture.find(type);
        if (it != effectToTexture.end()) {
            return "textures/ui/" + it->second;
        } else {
            return "Unknown Effect";
        }
    }

    static std::string effectTypeToString(EffectType type) {
        switch (type) {
            case EffectType::Speed: return "Speed";
            case EffectType::Slowness: return "Slowness";
            case EffectType::Haste: return "Haste";
            case EffectType::MiningFatigue: return "Mining Fatigue";
            case EffectType::Strength: return "Strength";
            case EffectType::InstantHealth: return "Instant Health";
            case EffectType::InstantDamage: return "Instant Damage";
            case EffectType::JumpBoost: return "Jump Boost";
            case EffectType::Nausea: return "Nausea";
            case EffectType::Regeneration: return "Regeneration";
            case EffectType::Resistance: return "Resistance";
            case EffectType::FireResistance: return "Fire Resistance";
            case EffectType::WaterBreathing: return "Water Breathing";
            case EffectType::Invisibility: return "Invisibility";
            case EffectType::Blindness: return "Blindness";
            case EffectType::NightVision: return "Night Vision";
            case EffectType::Hunger: return "Hunger";
            case EffectType::Weakness: return "Weakness";
            case EffectType::Poison: return "Poison";
            case EffectType::Wither: return "Wither";
            case EffectType::HealthBoost: return "Health Boost";
            case EffectType::Absorption: return "Absorption";
            case EffectType::Saturation: return "Saturation";
            case EffectType::Levitation: return "Levitation";
            case EffectType::FatalPoison: return "Fatal Poison";
            case EffectType::ConduitPower: return "Conduit Power";
            case EffectType::SlowFalling: return "Slow Falling";
            case EffectType::BadOmen: return "Bad Omen";
            case EffectType::VillageHero: return "Village Hero";
            case EffectType::Darkness: return "Darkness";
            case EffectType::TrialOmen: return "Trial Omen";
            case EffectType::WindCharged: return "Wind Charged";
            case EffectType::Weaving: return "Weaving";
            case EffectType::Oozing: return "Oozing";
            case EffectType::Infested: return "Infested";
            case EffectType::RaidOmen: return "Raid Omen";
            default: return "Unknown Effect";
        }
    }

    static std::string amplifierToString(int amplifier) {
        switch (amplifier) {
            case 0: return "I";
            case 1: return "II";
            case 2: return "III";
            case 3: return "IV";
            case 4: return "V";
            default: return FlarialGUI::cached_to_string(amplifier + 1);
        }
    }
};

class MobEffectInstance1_21_20 {
public:
    MobEffectInstance1_21_20() = delete;

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
    char pad_0064[0x64];
};

static_assert(sizeof(MobEffectInstance1_21_20) == 0x80);

class MobEffectInstance1_21_30 {
public:
    MobEffectInstance1_21_30() = delete;

    MobEffect::EffectType id;
    int duration;
    float idk;
    int durationEasy;
    int durationNormal;
    int durationHard;
    char pad_0008[0x8];
    int amplifier;
    bool displayOnScreenTextureAnimation;
    bool ambient;
    bool noCounter;
    bool effectVisible;
    char pad_0060[0x60];
};

static_assert(sizeof(MobEffectInstance1_21_30) == 0x88);

struct UnifiedMobEffectData {
    MobEffect::EffectType id;
    int duration;
    int amplifier;

    UnifiedMobEffectData(MobEffect::EffectType id, int duration, int amplifier) : id(id), duration(duration), amplifier(amplifier) {
    }

    [[nodiscard]] bool isValid() const {
        return id != MobEffect::EffectType::Empty;
    }

    [[nodiscard]] std::string getName() const {
        return MobEffect::effectTypeToString(id);
    }

    [[nodiscard]] int getAmplifier() const {
        return amplifier + 1;
    }

    [[nodiscard]] std::string getNameAndTime() const {
        return getName() + MobEffect::amplifierToString(getAmplifier()) + "\n" + getTime();
    }

    [[nodiscard]] std::string getTime() const {
        const int &totalSeconds = duration / 20;

        const int &minutes = totalSeconds / 60;
        const int &seconds = totalSeconds % 60;

        return std::format("{}:{:02}", minutes, seconds);
    }

    [[nodiscard]] std::string getTexturePath() const {
        return MobEffect::effectTypeToPath(id);
    }

    [[nodiscard]] ResourceLocation getTextureLocation() const {
        auto location = ResourceLocation(getTexturePath(), false);
        return location;
    }

    [[nodiscard]] std::pair<bool, int> hasTexture() const {
        return MobEffect::hasTexture(id);
    }
};

struct MobEffectsComponent1_21_20 {
    static constexpr hat::fixed_string type_name = "struct MobEffectsComponent";
    std::vector<MobEffectInstance1_21_20> effects;
};

struct MobEffectsComponent1_21_30 {
    static constexpr hat::fixed_string type_name = "struct MobEffectsComponent";
    std::vector<MobEffectInstance1_21_30> effects;
};

class MobEffectInstance {
};

struct MobEffectsComponent : IEntityComponent {
private:
    std::vector<MobEffectInstance> effects;

public:
    [[nodiscard]] std::vector<UnifiedMobEffectData> getUnifiedEffects() const {
        std::vector<UnifiedMobEffectData> unifiedEffects;
        if (VersionUtils::checkAboveOrEqual(21, 30)) {
            auto &_effects = hat::member_at<std::vector<MobEffectInstance1_21_30> >(this, 0x0);
            for (auto &effect: _effects) {
                unifiedEffects.emplace_back(effect.id, effect.duration, effect.amplifier);
            }
        } else {
            auto &_effects = hat::member_at<std::vector<MobEffectInstance1_21_20> >(this, 0x0);

            for (auto &effect: _effects) {
                unifiedEffects.emplace_back(effect.id, effect.duration, effect.amplifier);
            }
        }
        return unifiedEffects;
    }
};

static_assert(sizeof(MobEffectsComponent) == 0x18);
