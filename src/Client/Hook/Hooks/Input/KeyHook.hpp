#pragma once

#include <array>
#include "../Hook.hpp"

class KeyHook : public Hook {
private:
    static inline std::array<bool, 256> keys;

    static void keyCallback(int key, bool state);

public:
    typedef void(__thiscall *original)(int keyId, bool held);

    static inline original funcOriginal = nullptr;

    KeyHook();

    void enableHook() override;
};
