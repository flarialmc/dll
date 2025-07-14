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
    unsigned int uvOffset1,
    unsigned int uvOffset2,
    float uvRot1,
    float uvRot2,
    BaseActorRenderContext **glintUVScale,
    MCCColor *uvAnim,
    float br,
    unsigned char *lightEmission
) {
    auto event = nes::make_holder<ActorShaderParamsEvent>(entity, overlay, changeColor, glintColor, lightEmission);
    eventMgr.trigger(event);
    overlay = event->getOverlay();
    changeColor = event->getChangeColor();
    glintColor = event->getGlintColor();
    lightEmission = event->getLightEmission();
    return funcOriginal(screenContext, entityContext, entity, overlay, changeColor, changeColor2, glintColor, uvOffset1, uvOffset2, uvRot1, uvRot2, glintUVScale, uvAnim, br, lightEmission);
}
