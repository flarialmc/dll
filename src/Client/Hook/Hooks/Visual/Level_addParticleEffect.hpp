#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "SDK/Client/Core/HashedString.hpp"
#include "SDK/Client/Level/Level.hpp"

class Level_addParticleEffect : public Hook {
private:
    static void Level_addParticleEffectDetour(Level* level, void* id, void* pos, void* dir, void* data, void* tag, bool isGlobal);

public:
    typedef void(__thiscall *addParticleEffectOriginal)(Level* level, void* id, void* pos, void* dir, void* data, void* tag, bool isGlobal);

    static inline addParticleEffectOriginal funcOriginal = nullptr;

    Level_addParticleEffect();

    static void enableHookk();
};


