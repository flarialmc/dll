#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Events/Render/FogColorEvent.hpp"

class FogColorHook : public Hook {
private:

    static float *fogColorCallback(void *a1, void *a2, void *a3, void *a4) {

        auto color = MCCColor(funcOriginal(a1, a2, a3, a4));

        FogColorEvent event(color);
        EventHandler::onGetFogColor(event);

        return event.getFogColor().arr;
    }

public:
    typedef float *(__thiscall *fogColorOriginal)(void *, void *, void *, void *);

    static inline fogColorOriginal funcOriginal = nullptr;

    FogColorHook() : Hook("Fog Color Hook",
                          "41 0F 10 08 48 8B C2 0F 28 D3 F3 0F 59 1D ? ? ? ? F3 0F 59 15 ? ? ? ? F3 0F 58 1D ? ? ? ? 0F 11 0A F3 0F 58 15 ? ? ? ? F3 0F 59 5A ?") {}

    void enableHook() override {

        this->autoHook((void *) fogColorCallback, (void **) &funcOriginal);

    }
};


