#include "HurtColorHook.hpp"

MCCColor* HurtColorHook::HurtColorCallback(void* a1, MCCColor* color, void* a3)
{
    auto event = nes::make_holder<HurtColorEvent>(funcOriginal(a1, color, a3));

    if(VersionUtils::checkAboveOrEqual(21,40)) {
        auto actor = hat::member_at<Actor*>(a3, 0x38);
        if(actor && actor->getHurtTime() > 0) {
            eventMgr.trigger(event);
        }
    } else {
        eventMgr.trigger(event);
    }

    return event->getHurtColor();

}

HurtColorHook::HurtColorHook(): Hook("Hurt Color Hook", GET_SIG_ADDRESS("HurtColor"))
{}

void HurtColorHook::enableHook()
{
    auto RealFunc = Memory::offsetFromSig(address, 1);

    this->manualHook((void *) RealFunc, (void*)HurtColorCallback, (void **) &funcOriginal);
}
