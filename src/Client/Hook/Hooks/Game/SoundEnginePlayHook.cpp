#include "SoundEnginePlayHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/Game/SoundEnginePlayEvent.hpp"

uint64_t SoundEnginePlayHook::callback(void* SoundEngine, const std::string& name, Vec3<float> pos, float volume, float a5) {
  auto event = nes::make_holder<SoundEnginePlayEvent>(name, pos, volume, a5);
  eventMgr.trigger(event);

    // Logger::debug("Sound played: {} at ({}, {}, {}). volume: {}, a5: {}", name, pos.x, pos.y, pos.z, volume, a5);
  return funcOriginal(SoundEngine, name, pos, volume, a5);
}


SoundEnginePlayHook::SoundEnginePlayHook() : Hook("SoundEnginePlayHook", GET_SIG_ADDRESS("SoundEngine::play")) {}


void SoundEnginePlayHook::enableHook() {
    this->autoHook((void *) callback, (void **) &funcOriginal);
}
