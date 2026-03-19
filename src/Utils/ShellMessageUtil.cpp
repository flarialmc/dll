#include "ShellMessageUtil.hpp"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

// ShellMessageBoxW signature (not always in headers, but exported from shlwapi.dll).
// Important: this is a variadic Win32 API, so the function pointer must use WINAPIV.
// For the wide version, "%s" is the correct wsprintf-style format token for LPCWSTR.

int ShellMessageUtil::showW(HWND hwnd, const wchar_t* text, const wchar_t* caption, UINT type) {
    HMODULE shlwapi = GetModuleHandleW(L"shlwapi.dll");
    if (!shlwapi) shlwapi = LoadLibraryW(L"shlwapi.dll");

    if (shlwapi) {
        using ShellMessageBoxW_t = int (WINAPIV*)(HINSTANCE, HWND, LPCWSTR, LPCWSTR, UINT, ...);
        auto pShellMessageBoxW = reinterpret_cast<ShellMessageBoxW_t>(
            GetProcAddress(shlwapi, "ShellMessageBoxW")
        );
        if (pShellMessageBoxW) {
            return pShellMessageBoxW(nullptr, hwnd, L"%s", caption, type, text);
        }
    }

    return MessageBoxW(hwnd, text, caption, type);
}

int ShellMessageUtil::showA(HWND hwnd, const char* text, const char* caption, UINT type) {
    // Convert to wide and delegate to showW
    int textLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    int capLen = MultiByteToWideChar(CP_UTF8, 0, caption, -1, nullptr, 0);

    std::wstring wText(textLen, L'\0');
    std::wstring wCaption(capLen, L'\0');

    MultiByteToWideChar(CP_UTF8, 0, text, -1, wText.data(), textLen);
    MultiByteToWideChar(CP_UTF8, 0, caption, -1, wCaption.data(), capLen);

    return showW(hwnd, wText.c_str(), wCaption.c_str(), type);
}
