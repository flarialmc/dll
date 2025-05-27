#pragma once

#include "../Module.hpp"

class DisableMouseWheel : public Module {
public:
    DisableMouseWheel() : Module("Disable Mouse Wheel", "Prevents you from accidentally scrolling through your hotbar", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
        Listen(this, MouseEvent, &DisableMouseWheel::onMouse)
    }

    void onEnable() override {
        Module::onEnable();
    }

    void onDisable() override {
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

    void onMouse(MouseEvent& event) {
        if (this->isEnabled() && event.getButton() == MouseButton::Scroll && SDK::getCurrentScreen() == "hud_screen") event.setCancelled(true);
    }
};
