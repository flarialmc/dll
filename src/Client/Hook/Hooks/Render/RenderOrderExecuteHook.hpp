#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RenderOrderExecuteHook : public Hook {
private:
    static void rendergraph_RenderOrder_execute(void *_this, void* a1, void* a2) {
        if (Client::disable) return;
        auto event = nes::make_holder<RenderOrderExecuteEvent>();
        eventMgr.trigger(event);
        if(!event->isCancelled())
            funcOriginal(_this, a1, a2);
    }

public:
    typedef void(__thiscall *original)(void *_this, void* a1, void* a2);

    static inline original funcOriginal = nullptr;

    RenderOrderExecuteHook() : Hook("rendergraph::RenderOrder::executeHook", GET_SIG_ADDRESS("rendergraph::RenderOrder::execute")) {}

    void enableHook() override {
        this->autoHook((void *) rendergraph_RenderOrder_execute, (void **) &funcOriginal);
    }
};