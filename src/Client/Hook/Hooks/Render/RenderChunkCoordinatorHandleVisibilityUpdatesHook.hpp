#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class RenderChunkCoordinatorHandleVisibilityUpdatesHook : public Hook {
private:
    static void RenderChunkCoordinator_handleVisibilityUpdates(void *_this) {
        auto event = nes::make_holder<HandleVisibilityUpdatesEvent>();
        eventMgr.trigger(event);
        if(!event->isCancelled())
            funcOriginal(_this);
    }

public:
    typedef void(__thiscall *original)(void *_this);

    static inline original funcOriginal = nullptr;

    RenderChunkCoordinatorHandleVisibilityUpdatesHook() : Hook("RenderChunkCoordinator::_handleVisibilityUpdates", Memory::offsetFromSig(GET_SIG_ADDRESS("RenderChunkCoordinator::_handleVisibilityUpdates"), 1)) {}

    void enableHook() override {
        this->autoHook((void *) RenderChunkCoordinator_handleVisibilityUpdates, (void **) &funcOriginal);
    }
};