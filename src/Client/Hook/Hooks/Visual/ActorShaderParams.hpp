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
        void *uvOffset1,
        void *uvOffset2,
        void *uvRot1,
        void *uvRot2,
        void **glintUVScale,
        void *uvAnim,
        float brightness,
        void *lightEmission
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
        void *uvOffset1,
        void *uvOffset2,
        void *uvRot1,
        void *uvRot2,
        void **glintUVScale,
        void *uvAnim,
        float brightness,
        void *lightEmission
    );

    static inline ActorShaderParamsOriginal funcOriginal = nullptr;

    ActorShaderParamsHook();

    void enableHook() override;
};
