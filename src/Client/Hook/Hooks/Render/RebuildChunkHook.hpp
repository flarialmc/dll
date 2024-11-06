#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RebuildChunkHook : public Hook {
private:
    static void RebuildChunk(void *_this) {
        auto event = nes::make_holder<RebuildChunkEvent>();
        eventMgr.trigger(event);
        if(!event->isCancelled())
            funcOriginal(_this);
    }

public:
    typedef void(__thiscall *original)(void *_this);

    static inline original funcOriginal = nullptr;

    RebuildChunkHook() : Hook("RebuildChunk",  Memory::offsetFromSig(GET_SIG_ADDRESS("RebuildChunk"), 1)) {}

    void enableHook() override {
        this->autoHook((void *) RebuildChunk, (void **) &funcOriginal);
    }
};