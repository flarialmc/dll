#pragma once

#include <array>
#include "../Hook.hpp"

class BobHurtHook : public Hook {
private:
    static void BobHurtCallback(void *_this, glm::mat4* matrix);

public:
    typedef void(__thiscall *original)(void *_this, glm::mat4* matrix);

    static inline original funcOriginal = nullptr;

    BobHurtHook();

    void enableHook() override;
};
