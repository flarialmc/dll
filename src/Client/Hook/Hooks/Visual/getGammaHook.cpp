#include "getGammaHook.hpp"

float getGammaHook::getGammaCallback(uintptr_t a1)
{

    if (!Options::isInitialized()) {
        Options::initialize(a1);
    }

    auto event = nes::make_holder<GammaEvent>(funcOriginal(a1));
    eventMgr.trigger(event);

    return event->getGamma();
}

getGammaHook::getGammaHook(): Hook("getGammaHook", GET_SIG_ADDRESS("Options::getGamma"))
{}

void getGammaHook::enableHook()
{
    this->autoHook((void *) getGammaCallback, (void **) &funcOriginal);
}
