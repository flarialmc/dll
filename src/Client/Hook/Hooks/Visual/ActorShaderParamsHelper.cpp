#include "ActorShaderParamsHelper.hpp"

ActorShaderParamsHelperHook::ActorShaderParamsHelperHook(): Hook("Actor Shader Params Helper Hook", GET_SIG_ADDRESS("ActorShaderManager::setupShaderParameterHelper")) {
}

void ActorShaderParamsHelperHook::enableHook() {
    this->autoHook((void *) ActorShaderParamsHelperCallback, (void **) &funcOriginal);
}

__int64 ActorShaderParamsHelperHook::ActorShaderParamsHelperCallback(
    void *shaderConstantManager,
    void *a2,
    MCCColor color1,
    void *a4,
    MCCColor overlay,
    MCCColor changeColor,
    MCCColor changeColor2,
    MCCColor glintColor,
    void *a8,
    void *a9,
    void *a10,
    unsigned int uvOffset1,
    unsigned int uvOffset2,
    float uvRot1,
    float uvRot2
) {
    // if (entity->getHurtTime() > 0) {
    //     // overlay->r = 0.f;
    //     // overlay->g = 0.f;
    //     // overlay->b = 1.f;
    //     // overlay->a = 0.7f;
    //     Logger::debug("{} {} {}", overlay->r, overlay->g, overlay->b);
    // }
    // Logger::debug("{} {} {}", changeColor2->r, changeColor2->g, changeColor2->b);
    // Vec2<float> lol = Vec2<float>{1.f, 1.f};
    // glintUVScale = &lol;
    // overlay->r = 0.f;
    // overlay->g = 0.f;
    // overlay->b = 1.f;
    // overlay->a = 0.7f;

    // MCCColor omg = MCCColor(0.f, 0.f, 1.f, 1.f);
    // color1.r = 0.f;
    // color1.g = 0.f;
    // color1.b = 1.f;
    // color1.a = 1.f;
    //
    // overlay.r = 0.f;
    // overlay.g = 0.f;
    // overlay.b = 1.f;
    // overlay.a = 1.f;
    //
    // a4->r = 0.f;
    // a4->g = 0.f;
    // a4->b = 1.f;
    // a4->a = 1.f;

    // Logger::debug("{} {} {} {}", color1.r, color1.g, color1.b, color1.a);
    // return 1;

    return funcOriginal(shaderConstantManager, a2, color1, a4, overlay, changeColor, changeColor2, glintColor, a8, a9, a10, uvOffset1, uvOffset2, uvRot1, uvRot2);
}
