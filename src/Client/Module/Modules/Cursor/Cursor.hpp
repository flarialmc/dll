#pragma once

#include <Utils/WinrtUtils.hpp>

#include "../Module.hpp"


class Cursor : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last;
    std::map<std::string, BedrockTextureData*> textures{};
public:

    Cursor() : Module("Cursor Tweaks", "Changes the mouse cursor\nwhen a gui element is hovered", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, DrawNineSliceEvent, &Cursor::onNineSliceDraw)
        Listen(this, DrawImageEvent, &Cursor::onDrawImage)
        Listen(this, GetTextureEvent, &Cursor::onGetTexture)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, DrawNineSliceEvent, &Cursor::onNineSliceDraw)
        Deafen(this, DrawImageEvent, &Cursor::onDrawImage)
        Deafen(this, GetTextureEvent, &Cursor::onGetTexture)
        textures.clear();
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
                WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::IBeam);
            }
            else
            {
                WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::Hand);
            }
            last = std::chrono::high_resolution_clock::now();
        }
        /*else if (event.getTexturePtr()->GetFilePath().contains("ScrollRail"))
        {
            Client::changeCursor(winrt::Windows::UI::Core::CoreCursorType::SizeNorthSouth);
            last = std::chrono::high_resolution_clock::now();
        }*/
        else if (SDK::currentScreen != "hud_screen")
        {
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last;
            if (duration.count() >= 0.01) {
                WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            }
        }
    }

    void onGetTexture(GetTextureEvent& event) {
        auto path = event.location->filePath;

        if (SDK::containsIgnoreCase(path, "hover") && !SDK::containsIgnoreCase(path, "nohover")) {
            textures[path] = event.textureData;
        }
    }

    void onDrawImage(DrawImageEvent& event) {
        if(SDK::currentScreen == "hud_screen") return;
        Vec2<float> mouse(MC::mousePos.x / 2, MC::mousePos.y / 2);
        if(VersionUtils::checkAboveOrEqual(21, 50)) {
            for(auto texture : textures) {
                if(event.getTextureData() == texture.second) {
                    if (texture.first.contains("edit_box")) {
                        WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::IBeam);
                    } else {
                        WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::Hand);
                    }
                    last = std::chrono::high_resolution_clock::now();
                }
            }
        } else {
            if (SDK::containsIgnoreCase(event.getTexturePath(), "hover") &&
                !SDK::containsIgnoreCase(event.getTexturePath(), "nohover")) {
                if (event.getTexturePath().contains("edit_box")) {
                    WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::IBeam);
                } else {
                    WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::Hand);
                }
                last = std::chrono::high_resolution_clock::now();
            }
        }
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last;
        if (duration.count() >= 0.01) {
            WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType::Arrow);
        }
    }
};
