#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class getFovHook : public Hook {
private:

    static float getFovCallback(void *a1, float f, void *a3, void *a4);

public:
    typedef float(__thiscall *getFovOriginal)(void *, float, void *, void *);

    static inline getFovOriginal funcOriginal = nullptr;

    getFovHook();

    void enableHook() override;
};