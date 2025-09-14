#include "Level_addParticleEffect.hpp"

#include "SDK/SDK.hpp"

void Level_addParticleEffect::Level_addParticleEffectDetour(Level* level, void* id, void* pos, void* dir, void* data, void* tag, bool isGlobal)
{
    for (int i = 0; i < 100; i++) {
        funcOriginal(level, id, pos, dir, data, tag, isGlobal);
    }
    return funcOriginal(level, id, pos, dir, data, tag, isGlobal);
}

Level_addParticleEffect::Level_addParticleEffect(): Hook("Add Particle Effects Hook", 0x0)
{}

void Level_addParticleEffect::enableHookk()
{
    Memory::hookFunc(SDK::clientInstance->getLocalPlayer()->getLevel()->vtable[242], (void*)&Level_addParticleEffect::Level_addParticleEffectDetour, (void**)&funcOriginal, "Add Particle Effects Hook");
}
