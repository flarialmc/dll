#include "Level_sendServerLegacyParticle.hpp"

#include "SDK/SDK.hpp"

#include <random> // for random offsets

void Level_sendServerLegacyParticle::Level_sendServerLegacyParticleDetour(Level* level, ParticleType id, const Vec3<float>& pos, const Vec3<float>& dir, void* data, void* tag, bool isGlobal)
{
    Vec3<float> originalPos = static_cast<Vec3<float>>(pos);
    Vec3<float> originalDir = static_cast<Vec3<float>>(dir);

    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

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


Level_sendServerLegacyParticle::Level_sendServerLegacyParticle(): Hook("Send Server Legacy Particle Hook", 0x0)
{}

void Level_sendServerLegacyParticle::enableHookk()
{
    Memory::hookFunc(SDK::clientInstance->getLocalPlayer()->getLevel()->vtable[175], (void*)&Level_sendServerLegacyParticle::Level_sendServerLegacyParticleDetour, (void**)&funcOriginal, "Send Server Legacy Particle Hook");
}