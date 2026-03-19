#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

// Themed message box wrapper.
// Prefers ShellMessageBoxW for OS-native themed dialogs, falls back to MessageBoxW.
namespace ShellMessageUtil {
    int showW(HWND hwnd, const wchar_t* text, const wchar_t* caption, UINT type);
    int showA(HWND hwnd, const char* text, const char* caption, UINT type);
}
