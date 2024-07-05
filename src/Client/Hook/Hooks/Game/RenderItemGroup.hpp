#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"

class RenderItemGroupHook : public Hook {

private:
    static void callback(void * thisptr, BaseActorRenderContext* renderContext, Actor* itemEntity, int itemCount, float scaleValue, float a6);

public:
    typedef void(__thiscall *original)(void *, void*, Actor*, int, float, float);

    static inline original funcOriginal = nullptr;

    RenderItemGroupHook() : Hook("RenderItemGroup","45 85 C9 0F 8E A9 03") {}

    void enableHook() override;
};