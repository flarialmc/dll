#pragma once

#include <array>
#include "../Hook.hpp"

class KeyHook : public Hook {

    static inline std::array<bool, 256> keys;
public:
    static void keyCallback(int key, bool state);
    static bool handle(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Returns true if event was cancelled

public:
    typedef void(__thiscall *original)(int keyId, bool held);

    static inline original funcOriginal = nullptr;
    static inline LONG_PTR WndProcOriginal = 0;

    KeyHook();

    void enableHook() override;
};
