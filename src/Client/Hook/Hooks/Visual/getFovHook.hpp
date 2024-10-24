#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/EventManager.hpp"

class getFovHook : public Hook {
private:

    static float getFovCallback(void *a1, float f, void *a3, void *a4) {

        float fov = funcOriginal(a1, f, a3, a4);

        auto event = nes::make_holder<FOVEvent>(fov);
        eventMgr.trigger(event);

        return event->getFOV();
    }

public:
    typedef float(__thiscall *getFovOriginal)(void *, float, void *, void *);

    static inline getFovOriginal funcOriginal = nullptr;

    getFovHook() : Hook("getFovHook", GET_SIG_ADDRESS("LevelRendererPlayer::getFov")) {}

    void enableHook() override {
        this->autoHook((void *) getFovCallback, (void **) &funcOriginal);
    }
};