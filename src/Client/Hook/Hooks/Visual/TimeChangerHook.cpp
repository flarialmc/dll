#include "TimeChangerHook.hpp"

float TimeChangerHook::TimeChangerCallback(void* a1, void* a2, void* a3)
{

    float time = func_original(a1, a2, a3);

    auto event = nes::make_holder<TimeEvent>(time);
    eventMgr.trigger(event);

    return event->getTime();
}

TimeChangerHook::TimeChangerHook(): Hook("Time Changer Hook", GET_SIG_ADDRESS("TimeChanger"))
{}

void TimeChangerHook::enableHook()
{
    this->autoHook((void *) TimeChangerCallback, (void **) &func_original);
}
