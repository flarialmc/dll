#include "BobHurt.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

BobHurtHook::BobHurtHook() : Hook("BobHurt", GET_SIG_ADDRESS("BobHurt")) {}

void BobHurtHook::enableHook() {
    this->autoHook((void *) BobHurtCallback, (void **) &funcOriginal);
}

void BobHurtHook::BobHurtCallback(void *_this, glm::mat4* matrix) {
    auto event = nes::make_holder<BobHurtEvent>(matrix);
    eventMgr.trigger(event);

    return funcOriginal(_this, event->matrix);
}