#pragma once

#include <windows.h>
#include <string>
#include <thread>

class WindowManager {
public:
    static WindowManager& instance();

    // Accessors
    [[nodiscard]] HWND getWindow() const { return window_; }
    [[nodiscard]] HMODULE getCurrentModule() const { return currentModule_; }
    [[nodiscard]] std::thread::id getMainThreadId() const { return mainThreadId_; }

    // Mutators
    void setWindow(HWND hwnd) { window_ = hwnd; }
    void setCurrentModule(HMODULE module) { currentModule_ = module; }
    void setMainThreadId(std::thread::id id) { mainThreadId_ = id; }

    // Window operations
    void setWindowTitle(const std::string& title);
    void centerCursor();
    void findGameWindow();

    // Delete copy/move
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

private:
    WindowManager() = default;
    ~WindowManager() = default;

    HWND window_ = nullptr;
    HMODULE currentModule_ = nullptr;
    std::thread::id mainThreadId_;
};
