#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"

class ActorShaderParamsHook : public Hook {
private:
    static void ActorShaderParamsCallback(
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
        unsigned __int8 *lightEmission
    );

public:
    typedef void (__thiscall *ActorShaderParamsOriginal)(
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
        unsigned __int8 *lightEmission
    );

    static inline ActorShaderParamsOriginal funcOriginal = nullptr;

    ActorShaderParamsHook();

    void enableHook() override;
};
