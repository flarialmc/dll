#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../Events/EventHandler.hpp"

class getFovHook : public Hook {
private:

    static float getFovCallback(void *a1, float f, void *a3, void *a4) {

        float fov = funcOriginal(a1, f, a3, a4);
        FOVEvent event(fov);
        EventHandler::onGetFOV(event);
        return event.getFOV();
    }

public:
    typedef float(__thiscall *getFovOriginal)(void *, float, void *, void *);

    static inline getFovOriginal funcOriginal = nullptr;

    getFovHook() : Hook("getFovHook",
                        "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F") {}

    void enableHook() override {
        this->autoHook((void *) getFovCallback, (void **) &funcOriginal);
    }
};