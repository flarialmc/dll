#pragma once

#include "../Module.hpp"

class DisableMouseWheel : public Module {
public:
    DisableMouseWheel() : Module("Disable Mouse Wheel", "Prevents you from accidentally scrolling through your hotbar",
                                 IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, MouseEvent, &DisableMouseWheel::onMouse)
        Listen(this, KeyEvent, &DisableMouseWheel::onKey)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, MouseEvent, &DisableMouseWheel::onMouse)
        Deafen(this, KeyEvent, &DisableMouseWheel::onKey)
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

        this->addHeader("Main");
        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    bool disableScrolling = true;

    void onMouse(MouseEvent &event) {
        if (this->isEnabled() && event.getButton() == MouseButton::Scroll && SDK::getCurrentScreen() == "hud_screen" && disableScrolling)
            event.setCancelled(true);
    }

    void onKey(KeyEvent &event) {
        if (this->isKeybind(event.keys) &&
            this->isKeyPartOfKeybind(event.key) &&
            event.getAction() == ActionType::Pressed
        ) {
            disableScrolling = !disableScrolling;
        }
    };
};
