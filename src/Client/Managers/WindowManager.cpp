#include "WindowManager.hpp"
#include "ClientSettingsManager.hpp"
#include <Client/Client.hpp>
#include <Config/Settings.hpp>
#include <Utils/PlatformUtils.hpp>
#include <Utils/WinrtUtils.hpp>
#include <SDK/SDK.hpp>

WindowManager& WindowManager::instance() {
    static WindowManager instance;
    return instance;
}

void WindowManager::findGameWindow() {
    window_ = FindWindowA(nullptr, "Minecraft");

    // Grab the correct window for GDK builds
    HWND wnd = nullptr;
    DWORD currentId = GetCurrentProcessId();
    mainThreadId_ = std::this_thread::get_id();

    while ((wnd = FindWindowExW(nullptr, nullptr, L"Bedrock", nullptr))) {
        DWORD processId = 0;
        GetWindowThreadProcessId(wnd, &processId);

        if (currentId == processId) {
            window_ = wnd;
            break;
        }
    }
}

void WindowManager::setWindowTitle(const std::string& title) {
    if (PlatformUtils::isUWP()) {
        WinrtUtils::setWindowTitle(title);
    } else {
        SetWindowTextW(window_, std::wstring(title.begin(), title.end()).c_str());
    }
}

// Static variables for centerCursor thread management
static HWND s_hWnd = nullptr;
static RECT s_currentRect = {0};
static RECT s_clientRect = {0};
static RECT s_previousRect = {0};
static bool s_inHudScreen = false;
static bool s_cursorThreadRunning = false;

void WindowManager::centerCursor() {
    auto* centreCursorSetting = Client::settings.getSettingByName<bool>("centreCursor");

    if (!centreCursorSetting || !centreCursorSetting->value) return;

    if (s_hWnd == nullptr) {
        s_hWnd = FindWindowW(nullptr, L"Minecraft");
    }

    if (s_hWnd != nullptr) {
        if (!s_cursorThreadRunning) {
            s_cursorThreadRunning = true;
            std::thread cursorThread([this]() {
                auto* setting = Client::settings.getSettingByName<bool>("centreCursor");
                while (setting && setting->value) {
                    GetWindowRect(s_hWnd, &s_currentRect);
                    GetClientRect(s_hWnd, &s_clientRect);

                    if (memcmp(&s_currentRect, &s_previousRect, sizeof(RECT)) != 0) {
                        s_previousRect = s_currentRect;
                    }
                    Sleep(1000);
                }
                s_cursorThreadRunning = false;
            });
            cursorThread.detach();
        }

        std::string currentScreen = SDK::getCurrentScreen();
        if ((currentScreen != "hud_screen" && s_inHudScreen) ||
            (currentScreen == "hud_screen" && !s_inHudScreen)) {
            GetWindowRect(s_hWnd, &s_currentRect);
            GetClientRect(s_hWnd, &s_clientRect);

            int windowX = s_currentRect.left;
            int windowY = s_currentRect.top;

            int centerX = windowX + (s_clientRect.right) / 2;
            int centerY = windowY + (s_clientRect.bottom) / 2;

            SetCursorPos(centerX, centerY);

            s_inHudScreen = !s_inHudScreen;
        }
    }
}
