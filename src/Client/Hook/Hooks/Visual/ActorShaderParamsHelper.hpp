#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"

class ActorShaderParamsHelperHook : public Hook {
private:
    static __int64 ActorShaderParamsHelperCallback(
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
    );

public:
    typedef __int64 (__thiscall *ActorShaderParamsHelperOriginal)(
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
    );

    static inline ActorShaderParamsHelperOriginal funcOriginal = nullptr;

    ActorShaderParamsHelperHook();

    void enableHook() override;
};
