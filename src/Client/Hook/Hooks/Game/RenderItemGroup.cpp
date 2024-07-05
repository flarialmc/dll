#pragma once

#include "RenderItemGroup.hpp"

void RenderItemGroupHook::callback(void *thisptr, BaseActorRenderContext *renderContext, Actor *itemEntity, int itemCount,
                              float scaleValue, float a6) {
    funcOriginal(thisptr, renderContext, itemEntity, itemCount, scaleValue, a6);
}

void RenderItemGroupHook::enableHook() {
    this->autoHook((void *) callback, (void **) &funcOriginal);
}