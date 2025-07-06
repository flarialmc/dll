#include "ActorShaderParams.hpp"

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
    // Logger::debug("{} {} {}", changeColor2->r, changeColor2->g, changeColor2->b);
    Vec2<float> lol = Vec2<float>{1.f, 1.f};
    // glintUVScale = &lol;
    overlay->r = 0.f;
    overlay->g = 0.f;
    overlay->b = 1.f;
    overlay->a = 0.7f;
    Logger::debug("{} {} {}", overlay->r, overlay->g, overlay->b);
    return funcOriginal(screenContext, entityContext, entity, overlay, changeColor, changeColor2, glintColor, uvOffset1, uvOffset2, uvRot1, uvRot2, glintUVScale, uvAnim, br, lightEmission);
}
