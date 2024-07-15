#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RenderActorHook : public Hook {

private:
    static void callback(void *a1, void *a2, Actor *actor, Vec3<float> a4, Vec3<float> a5, Vec2<float> *a6, void *a7);

public:

    typedef void(__thiscall *original)(void *a1, void *a2, Actor *actor, Vec3<float> a4, Vec3<float> a5,
                                       Vec2<float> *a6, void *a7);

    static inline original renderActorOriginal = nullptr;

    RenderActorHook() : Hook("Render Actor", GET_SIG("RenderActor")) {}

    void enableHook() override;
};