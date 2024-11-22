#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CursorListener.hpp"


class Cursor : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last;
public:

    Cursor() : Module("Cursor Tweaks", "Changes the mouse cursor\nwhen a gui element is hovered", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
        EventHandler::registerListener(new CursorListener("Cursor", this));
    }

    void onEnable() override {
        Module::onEnable();
    }

    void onDisable() override {
        Module::onDisable();
    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Troll", textWidth * 3.0f, textHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::RelativeConstraint(0.12, "height", true),
            DWRITE_FONT_WEIGHT_NORMAL);
    }
};
