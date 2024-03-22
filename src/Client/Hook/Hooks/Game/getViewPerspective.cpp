#include "getViewPerspective.hpp"

int getViewPerspectiveHook::callback(uintptr_t *a1) {
    PerspectiveEvent event((Perspective)getViewPerspectiveOriginal(a1));
    EventHandler::onGetViewPerspective(event);
    return (int)event.getPerspective();
}

void getViewPerspectiveHook::enableHook() {
    this->autoHook((void *) callback, (void **) &getViewPerspectiveOriginal);
}
