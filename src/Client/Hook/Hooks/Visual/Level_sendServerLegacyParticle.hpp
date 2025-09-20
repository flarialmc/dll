#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Utils.hpp"
#include "SDK/Client/Level/Level.hpp"
#include "Level_addParticleEffect.hpp"

class Level_sendServerLegacyParticle : public Hook {
private:
    static void Level_sendServerLegacyParticleDetour(Level* level, ParticleType id, const Vec3<float>& pos, const Vec3<float>& dir, void* data, void* tag, bool isGlobal);

public:
    typedef void(__thiscall *sendServerLegacyParticleOriginal)(Level* level, ParticleType id, const Vec3<float>& pos, const Vec3<float>& dir, void* data, void* tag, bool isGlobal);

    static inline sendServerLegacyParticleOriginal funcOriginal = nullptr;

    Level_sendServerLegacyParticle();

    static void enableHookk();
};