#include "BeaconWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static constexpr const wchar_t* kClassName = L"Flarial Client";
static HWND sBeaconHwnd = nullptr;
static ATOM sClassAtom = 0;

static LRESULT CALLBACK beaconWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void BeaconWindow::create() {
    if (sBeaconHwnd) return;

    WNDCLASSW wc{};
    wc.lpfnWndProc = beaconWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClassName;

    sClassAtom = RegisterClassW(&wc);
    if (!sClassAtom) return;

    // HWND_MESSAGE creates a message-only window — invisible, no taskbar entry
    sBeaconHwnd = CreateWindowExW(
        0, kClassName, nullptr, 0,
        0, 0, 0, 0,
        HWND_MESSAGE, nullptr, wc.hInstance, nullptr
    );
}

void BeaconWindow::destroy() {
    if (sBeaconHwnd) {
        DestroyWindow(sBeaconHwnd);
        sBeaconHwnd = nullptr;
    }
    if (sClassAtom) {
        UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
        sClassAtom = 0;
    }
}
