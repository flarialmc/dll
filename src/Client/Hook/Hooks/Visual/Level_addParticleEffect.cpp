#include "Level_addParticleEffect.hpp"

#include "SDK/SDK.hpp"

#include <random> // for random offsets

void Level_addParticleEffect::Level_addParticleEffectDetour(Level* level, ParticleType id, const Vec3<float>& pos, const Vec3<float>& dir, void* data, void* tag, bool isGlobal)
{
    /*
    Vec3<float> originalPos = static_cast<Vec3<float>>(pos);
    Vec3<float> originalDir = static_cast<Vec3<float>>(dir);

    //Logger::debug("Particle Type: {}", static_cast<int>(id));
    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

*/
    /*
    for (int i = 0; i < 30; i++) {
        Vec3<float> offsetPos = originalPos;
        offsetPos.x += dist(rng);
        offsetPos.y += dist(rng);
        offsetPos.z += dist(rng);

        Vec3<float> offsetDir = originalDir;
        offsetDir.x += dist(rng) * 0.05f;
        offsetDir.y += dist(rng) * 0.05f;
        offsetDir.z += dist(rng) * 0.05f;

        funcOriginal(level, id, offsetPos, offsetDir, data, tag, isGlobal);
    }
*/
    return funcOriginal(level, id, pos, dir, data, tag, isGlobal);
}


Level_addParticleEffect::Level_addParticleEffect(): Hook("Add Particle Effects Hook", 0x0)
{}

void Level_addParticleEffect::enableHookk()
{
    Memory::hookFunc(SDK::clientInstance->getLocalPlayer()->getLevel()->vtable[242], (void*)&Level_addParticleEffect::Level_addParticleEffectDetour, (void**)&funcOriginal, "Add Particle Effects Hook");
}
