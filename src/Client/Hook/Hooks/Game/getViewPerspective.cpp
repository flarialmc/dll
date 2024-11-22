#include "getViewPerspective.hpp"

int getViewPerspectiveHook::callback(uintptr_t *a1) {
    auto event = nes::make_holder<PerspectiveEvent>((Perspective)getViewPerspectiveOriginal(a1));
    eventMgr.trigger(event);
    return (int)event->getPerspective();
}

void getViewPerspectiveHook::enableHook() {
    this->autoHook((void *) callback, (void **) &getViewPerspectiveOriginal);
}
