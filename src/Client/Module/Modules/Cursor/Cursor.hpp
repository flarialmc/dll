#pragma once

#include "../Module.hpp"


class Cursor : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last;
public:

    Cursor() : Module("Cursor Tweaks", "Changes the mouse cursor\nwhen a gui element is hovered", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, DrawNineSliceEvent, &Cursor::onNineSliceDraw)
        Listen(this, DrawImageEvent, &Cursor::onDrawImage)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, DrawNineSliceEvent, &Cursor::onNineSliceDraw)
        Deafen(this, DrawImageEvent, &Cursor::onDrawImage);
        Module::onDisable();
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onNineSliceDraw(DrawNineSliceEvent& event) {
        if (SDK::containsIgnoreCase(event.getTexturePtr()->GetFilePath(), "hover") && !SDK::containsIgnoreCase(event.getTexturePtr()->GetFilePath(), "nohover") && SDK::currentScreen != "hud_screen")
        {
            if (SDK::currentScreen == "chat_screen" && MC::mousePos.y > MC::windowSize.y / 2 && MC::mousePos.x > MC::windowSize.x / 10.85)
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::IBeam);
            }
            else {
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

    void onDrawImage(DrawImageEvent& event) {
        if (SDK::containsIgnoreCase(event.getTexturePath(), "hover") && !SDK::containsIgnoreCase(event.getTexturePath(), "nohover") && SDK::currentScreen != "hud_screen")
        {
            if (SDK::currentScreen == "chat_screen" && MC::mousePos.y > MC::windowSize.y / 2 && MC::mousePos.x > MC::windowSize.x / 10.85)
            {
                Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::IBeam);
            }
            else {
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
};
