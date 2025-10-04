#include "ActorShaderParams.hpp"
#include "Events/Render/ActorShaderParamsEvent.hpp"

ActorShaderParamsHook::ActorShaderParamsHook(): Hook("Actor Shader Params Hook", GET_SIG_ADDRESS("ActorShaderManager::setupShaderParameter")) {
}

void ActorShaderParamsHook::enableHook() {
    this->autoHook((void *) ActorShaderParamsCallback, (void **) &funcOriginal);
}

void ActorShaderParamsHook::ActorShaderParamsCallback(
    ScreenContext *screenContext,
    BaseActorRenderContext *entityContext,
    Actor *entity,
    MCCColor *overlay,
    MCCColor *changeColor,
    MCCColor *changeColor2,
    MCCColor *glintColor,
    void *uvOffset1,
    void *uvOffset2,
    void *uvRot1,
    void *uvRot2,
    void **glintUVScale,
    void *uvAnim,
    float brightness,
    void *lightEmission
) {
    auto event = nes::make_holder<ActorShaderParamsEvent>(overlay, changeColor, glintColor);
    eventMgr.trigger(event);

    //return funcOriginal(screenContext, entityContext, entity, event->getOverlay(), event->getChangeColor(), changeColor2, event->getGlintColor(), uvOffset1, uvOffset2, uvRot1, uvRot2, glintUVScale, uvAnim, brightness, lightEmission);
}