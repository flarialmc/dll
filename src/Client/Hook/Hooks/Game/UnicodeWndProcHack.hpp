//
// Created by Leslie on 12/31/2025.
//
#pragma once

#include <array>
#include <Windows.h>
#include "Client.hpp"
#include "../Hook.hpp"

class UnicodeWndProcHack : public Hook {
    static WNDPROC g_realWndProc;
    static LRESULT CALLBACK MasterWndProc(
        HWND, UINT, WPARAM, LPARAM
    );
public:
    static bool handle (
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam
    );

    UnicodeWndProcHack() : Hook("UnicodeWndProcHack", 0)
    {
    }

    void enableHook() override;
};
