#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RenderChunkCoordinatorPreRenderTickHook : public Hook {
private:
    static void RenderChunkCoordinator_preRenderTick(void *_this) {
        auto event = nes::make_holder<RenderChunkCoordinatorPreRenderTickEvent>();
        eventMgr.trigger(event);
        if(!event->isCancelled())
            funcOriginal(_this);
    }

public:
    typedef void(__thiscall *original)(void *_this);

    static inline original funcOriginal = nullptr;

    RenderChunkCoordinatorPreRenderTickHook() : Hook("RenderChunkCoordinator::preRenderTick",  Memory::offsetFromSig(GET_SIG_ADDRESS("RenderChunkCoordinator::preRenderTick"), 1)) {}

    void enableHook() override {
        this->autoHook((void *) RenderChunkCoordinator_preRenderTick, (void **) &funcOriginal);
    }
};