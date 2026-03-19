#pragma once

#include <format>
#include <windows.h>
#include <unknwn.h>
#include "../../../../Client.hpp"

class ImGUIMouseListener : public Listener {
public:
    void onMouse(MouseEvent &event) {
        // Multiple safety checks to prevent crashes during initialization/teardown
        if (!SwapchainHook::init.load()) return;
        if (!SwapchainHook::initImgui.load()) return;
        if (SwapchainHook::imguiCleanupInProgress.load()) return;

        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (!ctx) return;

        // Ensure IO is properly initialized before accessing
        ImGuiIO& io = ImGui::GetIO();
        if (!io.BackendPlatformUserData || !io.BackendRendererUserData) return;

        // Check if ImGui is currently accepting events (safe to access input queue)
        if (!io.AppAcceptingEvents) return;

        const int button = event.getButton();
        // ImGui expects button indices 0, 1, 2 (left, right, middle)
        // Our event uses 1, 2, 3, so we subtract 1
        if (button >= 1 && button <= 3) {
            std::lock_guard<std::mutex> lock(SwapchainHook::imguiInputMutex);
            io.AddMouseButtonEvent(button - 1, event.getAction() == 1);
        }
    };

    ImGUIMouseListener() {
        Listen(this, MouseEvent, &ImGUIMouseListener::onMouse);
    }

    ~ImGUIMouseListener() {
        Deafen(this, MouseEvent, &ImGUIMouseListener::onMouse);
    }
};
