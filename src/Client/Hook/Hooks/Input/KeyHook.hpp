#pragma once

#include <array>
#include "../Hook.hpp"

class KeyHook : public Hook
{
private:
    static inline std::array<bool, 256> keys;
    static void keyCallback(unsigned char key, int state);

public:
    typedef void(__thiscall *KeyOriginal)(unsigned char keyId, bool held);
    static inline KeyOriginal func_original = 0;

    KeyHook();
    void enableHook() override;
};
