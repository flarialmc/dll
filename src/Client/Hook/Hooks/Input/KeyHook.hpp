#pragma once

#include <array>
#include "../Hook.hpp"

class KeyHook : public Hook {
private:
    static inline std::array<bool, 256> keys;

    static void keyCallback(unsigned char key, int state);

public:
    typedef void(__thiscall *original)(unsigned char keyId, bool held);

    static inline original funcOriginal = nullptr;

    KeyHook();

    void enableHook() override;
};
