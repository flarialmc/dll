#pragma once
#include "../Hook.hpp"

class KeyHook : public Hook
{
private:
    static inline bool keys[256] = {false};

    static void keyCallback(int key, int state);

public:
    typedef void(__thiscall *KeyOriginal)(uint64_t keyId, bool held);
    static inline KeyOriginal func_original = 0;

    KeyHook();
    void enableHook() override;
};
