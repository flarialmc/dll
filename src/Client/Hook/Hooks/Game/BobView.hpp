#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class BobView : public Hook {
private:
    static void BobViewhook(uint64_t _this, glm::mat4* matrix) {
        auto event = nes::make_holder<BobViewEvent>();
        event->matrix = matrix;
        eventMgr.trigger(event);
    }

public:
    typedef void(__thiscall* original)(uint64_t _this, glm::mat4* matrix);

    static inline original funcOriginal = nullptr;

    BobView() : Hook("BobViewHook", GET_SIG_ADDRESS("BobView")) {}

    void enableHook() override {
        this->autoHook((void*)BobViewhook, (void**)&funcOriginal);
    }
};