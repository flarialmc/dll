#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class RenderLevelHook : public Hook {
private:
    static void callback(void *levelRenderer, void* scn, void* a3) {
        auto event = nes::make_holder<Render3DEvent>();
        eventMgr.trigger(event);



        funcOriginal(levelRenderer, scn, a3);
    }

public:
    typedef void(__thiscall *original)(void *levelRenderer, void* scn, void* a3);

    static inline original funcOriginal = nullptr;

    RenderLevelHook() : Hook("LevelRenderer::RenderLevel", GET_SIG_ADDRESS("LevelRenderer::renderlevel")) {}

    void enableHook() override {
        this->autoHook((void *) callback, (void **) &funcOriginal);
    }
};