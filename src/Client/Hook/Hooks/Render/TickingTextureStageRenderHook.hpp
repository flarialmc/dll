#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include <chrono>

class TickingTextureStageRenderHook : public Hook {
private:
    static inline std::chrono::steady_clock::time_point lastCallTime = std::chrono::steady_clock::now();
    static constexpr std::chrono::milliseconds minInterval{1000 / 20}; // 50ms = 20 calls per second

    static void TickingTextureStage__Render(class TickingTextureStage* _this, class ScreenContext* ctx, class FrameRenderObject* obj) {
        auto now = std::chrono::steady_clock::now();
        if (now - lastCallTime < minInterval)
            return;
        lastCallTime = now;
        return funcOriginal(_this, ctx, obj);
    }

public:
    typedef void(__thiscall *original)(TickingTextureStage *, class ScreenContext*, class FrameRenderObject*);

    static inline original funcOriginal = nullptr;

    TickingTextureStageRenderHook() : Hook("TickingTextureStage::render", GET_SIG_ADDRESS("TickingTextureStage::render")) {}

    void enableHook() override {
        this->autoHook((void *) TickingTextureStage__Render, (void **) &funcOriginal);
    }
};
