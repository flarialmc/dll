#pragma once

#include "../Module.hpp"

class SnapLook : public Module {
public:
    SnapLook() : Module("SnapLook", "Quickly look behind you.", IDR_EYE_PNG, "V") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, KeyEvent, &SnapLook::onKey)
        Listen(this, PerspectiveEvent, &SnapLook::onGetViewPerspective)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, KeyEvent, &SnapLook::onKey)
        Deafen(this, PerspectiveEvent, &SnapLook::onGetViewPerspective)
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

        this->addHeader("Misc");
        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    // TODO: make it togglable
    void onKey(KeyEvent &event) {
        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key))
            keybindActions[0]({});

        if (!this->isKeybind(event.keys)) this->active = false;
    };

    void onGetViewPerspective(PerspectiveEvent &event) {
        if (this->active)
            event.setPerspective(Perspective::ThirdPersonFront);
    }
};