#include "SoundEnginePlayHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/Game/SoundEnginePlayEvent.hpp"

uint64_t SoundEnginePlayHook::callback(void* SoundEngine, const std::string& name, Vec3<float> pos, float a4, float a5) {
  auto event = nes::make_holder<SoundEnginePlayEvent>(name, pos, a4, a5);
  eventMgr.trigger(event);

  Logger::debug(name);
  return funcOriginal(SoundEngine, name, pos, a4, a5);
}


SoundEnginePlayHook::SoundEnginePlayHook() : Hook("SoundEnginePlayHook", GET_SIG_ADDRESS("SoundEngine::play")) {}


void SoundEnginePlayHook::enableHook() {
    this->autoHook((void *) callback, (void **) &funcOriginal);
}
