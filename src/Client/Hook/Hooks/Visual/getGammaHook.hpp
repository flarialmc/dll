 #pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"
#include "../../../Client.hpp"

class getGammaHook : public Hook {
private:

    static float getGammaCallback(uintptr_t a1);

public:
    typedef float(__thiscall *getGammaOriginal)(uintptr_t);

    static inline getGammaOriginal funcOriginal = nullptr;

    getGammaHook();

    void enableHook() override;
};