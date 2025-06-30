#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class getViewPerspectiveHook : public Hook {

private:
    static int callback(uintptr_t *a1);

public:
    typedef int(__thiscall *original)(uintptr_t *);

    static inline original getViewPerspectiveOriginal = nullptr;

    getViewPerspectiveHook() : Hook("getViewPerspective", GET_SIG_ADDRESS("getViewPerspective")) {}

    void enableHook() override;
};