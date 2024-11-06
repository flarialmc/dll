#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RenderCurrentFrameHook : public Hook {
private:
    static void* GameRenderer_renderCurrentFrame(void *_this, void* a1, void* a2) {
        auto event = nes::make_holder<RenderCurrentFrameEvent>();
        eventMgr.trigger(event);
        if(!event->isCancelled())
            return funcOriginal(_this, a1, a2);
        return nullptr;
    }

public:
    typedef void*(__thiscall *original)(void *_this, void* a1, void* a2);

    static inline original funcOriginal = nullptr;

    // Memory::offsetFromSig(

    RenderCurrentFrameHook() : Hook("GameRenderer::renderCurrentFrameHook", GET_SIG_ADDRESS("GameRenderer::renderCurrentFrame")) {}

    void enableHook() override {
        this->autoHook((void *) GameRenderer_renderCurrentFrame, (void **) &funcOriginal);
    }
};