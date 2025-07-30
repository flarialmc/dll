#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/EventManager.hpp"

class ActorRenderDispatcher_Render : public Hook {
private:

    static void ActorRenderDispatcher_RenderCallback(void* a1, void* a2, void* a3) {
        auto event = nes::make_holder<ActorRenderDispatcherEvent>();
        eventMgr.trigger(event);
        funcOriginal(a1, a2, a3);
    }

public:
    typedef void(__thiscall* RenderLevelOriginal)(void* a1, void* a2, void* a3);

    static inline RenderLevelOriginal funcOriginal = nullptr;

    ActorRenderDispatcher_Render() : Hook("ActorRenderDispatcher_Render", GET_SIG_ADDRESS("ActorRenderDispatcher::render")) {}

    void enableHook() override {
        this->autoHook((void*)ActorRenderDispatcher_RenderCallback, (void**)&funcOriginal);
    }
};