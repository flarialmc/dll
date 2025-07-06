#include "TintColorHook.hpp"

MCCColor *TintColorHook::TintColorCallback(void *a1, MCCColor *color, void *a3) {
    // auto event = nes::make_holder<HurtColorEvent>(funcOriginal(a1, color, a3));
    //
    // if (VersionUtils::checkAboveOrEqual(21, 40)) {
    //     auto actor = hat::member_at<Actor *>(a3, 0x38);
    //     if (actor && actor->getHurtTime() > 0) {
    //         eventMgr.trigger(event);
    //     }
    // } else {
    //     eventMgr.trigger(event);
    // }
    //
    // return event->getHurtColor();
    auto actor = hat::member_at<Actor *>(a3, 0x38);
    if (actor && actor->getHurtTime() > 0) {
        Logger::debug("{} {} {} {}", color->r, color->g, color->b, color->a);
    }

    return funcOriginal(a1, color, a3);
}

TintColorHook::TintColorHook(): Hook("Tint Color Hook", GET_SIG_ADDRESS("RenderController::getTintColor")) {
}

void TintColorHook::enableHook() {
    auto RealFunc = Memory::offsetFromSig(address, 1);

    this->manualHook((void *) RealFunc, (void *) TintColorCallback, (void **) &funcOriginal);
}
