#include "getFovHook.hpp"

float getFovHook::getFovCallback(void* a1, float f, void* a3, void* a4)
{

    float fov = funcOriginal(a1, f, a3, a4);

    auto event = nes::make_holder<FOVEvent>(fov);
    eventMgr.trigger(event);

    return event->getFOV();
}

getFovHook::getFovHook(): Hook("getFovHook", GET_SIG_ADDRESS("LevelRendererPlayer::getFov"))
{}

void getFovHook::enableHook()
{
    this->autoHook((void *) getFovCallback, (void **) &funcOriginal);
}
