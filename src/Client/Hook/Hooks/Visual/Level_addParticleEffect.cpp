#include "Level_addParticleEffect.hpp"

#include "SDK/SDK.hpp"

void Level_addParticleEffect::Level_addParticleEffectDetour(Level* level, HashedString const& hashed_string, Vec3<float> pos, void* molangVariable)
{
    Logger::debug("Level_addParticleEffectDetour");
    return funcOriginal(level, hashed_string, pos, molangVariable);
}

Level_addParticleEffect::Level_addParticleEffect(): Hook("Add Particle Effects Hook", 0x0)
{}

void Level_addParticleEffect::enableHookk()
{
    Memory::hookFunc(SDK::clientInstance->getLocalPlayer()->getLevel()->vtable[243], (void*)&Level_addParticleEffect::Level_addParticleEffectDetour, (void**)&funcOriginal, "Add Particle Effects Hook");
}
