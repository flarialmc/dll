#pragma once

#include <format>
#include <windows.h>
#include <unknwn.h>
#include "../../../../Client.hpp"

class ImGUIMouseListener : public Listener {
public:
    void onMouse(MouseEvent &event) {
        if (!SwapchainHook::init || !ImGui::GetCurrentContext()) return;
        ImGuiIO& io = ImGui::GetIO();
        const int button = event.getButton();
        //io.AddMousePosEvent(event.getMouseX(), event.getMouseY());
        if (button >= 1 && button < 4) {
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
