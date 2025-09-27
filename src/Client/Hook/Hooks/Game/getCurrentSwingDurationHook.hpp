#pragma once
#include "../Hook.hpp"

class getCurrentSwingDurationHook : public Hook {

private:
    static int callback(void* mob);

public:
    typedef int(__thiscall *original)(void* mob);

    static inline original funcOriginal = nullptr;

    getCurrentSwingDurationHook();

    void enableHook() override;
};
