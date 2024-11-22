#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../Events/Render/DrawNineSliceEvent.hpp"
#include "../../../Events/Render/DrawImageEvent.hpp"
#include "../../../Client.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../Manager.hpp"
#include <windows.h>
#include <chrono>

class CursorListener : public Listener {
private:
    Module* module;
    std::chrono::time_point<std::chrono::high_resolution_clock> last;
    void onNineSliceDraw(DrawNineSliceEvent& event) {
        if (event.getTexturePtr()->GetFilePath().find("hover") && !event.getTexturePtr()->GetFilePath().find("nohover") && SDK::currentScreen != "hud_screen")
        {
            if (event.getTexturePtr()->GetFilePath().contains("edit_box"))
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::IBeam);
            }
            else
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
            }
            last = std::chrono::high_resolution_clock::now();
        }
        else if (SDK::currentScreen != "hud_screen")
        {
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last;
            if (duration.count() >= 0.01) {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            }
        }
    }

    void onDrawImage(DrawImageEvent& event) override {
        Vec2<float> mouse(MC::mousePos.x / 2, MC::mousePos.y / 2);
        if (event.getTexturePath().find("hover") && !event.getTexturePath().find("nohover") && SDK::currentScreen != "hud_screen")
        {
            if (event.getTexturePath().contains("edit_box"))
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::IBeam);
            }
            else
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
            }
            last = std::chrono::high_resolution_clock::now();
        }
        else if (SDK::currentScreen != "hud_screen")
        {
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last;
            if (duration.count() >= 0.01) {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            }
        }
    }

public:
    explicit CursorListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};

