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
        else if (event.getTexturePtr()->GetFilePath().contains("ScrollRail")  && SDK::isHovered(event.getPos(), Vec2<float>(MC::mousePos.x /2, MC::mousePos.y /2)))
        {
            Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::SizeNorthSouth);
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
        Vec2<float> mouse(MC::mousePos.x / 2, MC::mousePos.y / 2);
        if (SDK::containsIgnoreCase(event.getTexturePath(), "hover") && !SDK::containsIgnoreCase(event.getTexturePath(), "nohover") && SDK::currentScreen != "hud_screen")
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
};
