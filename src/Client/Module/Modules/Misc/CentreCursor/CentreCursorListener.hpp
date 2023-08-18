#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"
class CentreCursorListener : public Listener {

public:

    bool InHudScreen = false;

    void onLocalTick(TickEvent& event) override {
       
        if (SDK::CurrentScreen == "hud_screen" and !InHudScreen) InHudScreen = true;
        if (SDK::CurrentScreen != "hud_screen" and InHudScreen) {
            std::string window = "Minecraft";
            HWND hWnd = FindWindow(nullptr, window.c_str());

            if (hWnd != NULL) {
                RECT windowRect;
                GetWindowRect(hWnd, &windowRect);

                int windowX = windowRect.left;
                int windowY = windowRect.top;

                int centerX = windowX + Constraints::PercentageConstraint(0.5f, "left");
                int centerY = windowY + Constraints::PercentageConstraint(0.5f, "top");

                SetCursorPos(centerX, centerY);

                InHudScreen = false;
            }

        }

    }

public:
    explicit CentreCursorListener(const char string[5]) {
        this->name = string;
    }
};
