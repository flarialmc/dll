#pragma once
#include "../../../Events/Events.hpp"
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ApplyTurnDeltaHook : public Hook {
private:
    static void ApplyTurnDelta(void* a1, Vec2<float>& a2) {

        auto event = nes::make_holder<TurnDeltaEvent>(a1, a2);
        eventMgr.trigger(event);

        funcOriginal(a1, a2);
    }

public:
    typedef void(__thiscall* original)(void* a1, Vec2<float>& a2);

    static inline original funcOriginal = nullptr;

    ApplyTurnDeltaHook() : Hook("ApplyTurnDeltaHook", GET_SIG_ADDRESS("LocalPlayer::applyTurnDelta")) {}

    void enableHook() override {
        this->autoHook((void*)ApplyTurnDelta, (void**)&funcOriginal);
    }
};