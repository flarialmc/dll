// UnicodeWndProcHack.cpp
#include "UnicodeWndProcHack.hpp"

#include "Hook/Hooks/Input/KeyHook.hpp"

WNDPROC UnicodeWndProcHack::g_realWndProc = nullptr;

bool UnicodeWndProcHack::handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SETTEXT && !IsWindowUnicode(hWnd))
        return true;
    return false;
}

LRESULT UnicodeWndProcHack::MasterWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SETTEXT && !IsWindowUnicode(hWnd))
        return TRUE;

    // Check if key event was cancelled (e.g., when GUI is open)
    if (KeyHook::handle(hWnd, uMsg, wParam, lParam))
        return 0; // Block the key from reaching the game

    // Safety check - if g_realWndProc is null or points to ourselves, use DefWindowProcW
    if (!g_realWndProc || g_realWndProc == MasterWndProc)
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);

    return CallWindowProcW(g_realWndProc, hWnd, uMsg, wParam, lParam);
}

void UnicodeWndProcHack::enableHook()
{
    if (!g_realWndProc && Client::window)
    {
        WNDPROC currentProc = (WNDPROC)GetWindowLongPtrW(Client::window, GWLP_WNDPROC);

        // Don't hook if already hooked (prevents recursive hooks)
        if (currentProc == MasterWndProc)
            return;

        g_realWndProc = (WNDPROC)SetWindowLongPtrW(Client::window, GWLP_WNDPROC, (LONG_PTR)MasterWndProc);

        // Verify we didn't hook ourselves
        if (g_realWndProc == MasterWndProc)
            g_realWndProc = nullptr;
    }
}
