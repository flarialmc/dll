#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/Input/SensitivityEvent.hpp"

class ApplyTurnDeltaHook : public Hook {
private:
    static void ApplyTurnDelta(void* a1, Vec2<float>& delta) {
        // Fire SensitivityEvent first - modules can modify the multiplier
        float sensitivity = 1.0f;
        auto sensEvent = nes::make_holder<SensitivityEvent>(sensitivity);
        eventMgr.trigger(sensEvent);
        sensitivity = sensEvent->getSensitivity();

        // Apply sensitivity multiplier to delta
        delta.x *= sensitivity;
        delta.y *= sensitivity;

        // Fire TurnDeltaEvent for other modifications (cinematic smoothing, etc.)
        auto event = nes::make_holder<TurnDeltaEvent>(a1, delta);
        eventMgr.trigger(event);

        // Copy modified delta back (in case nes::make_holder doesn't preserve reference properly)
        delta = event->delta;

        funcOriginal(a1, delta);
    }

public:
    typedef void(__thiscall* original)(void* a1, Vec2<float>& delta);

    static inline original funcOriginal = nullptr;

    ApplyTurnDeltaHook() : Hook("ApplyTurnDeltaHook", GET_SIG_ADDRESS("LocalPlayer::applyTurnDelta")) {}

    void enableHook() override {
        this->autoHook((void*)ApplyTurnDelta, (void**)&funcOriginal);
    }
};