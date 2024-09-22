#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SneakListener.hpp"


class Sneak : public Module {

public:


    Sneak() : Module("Toggle Sneak", "No need to hold down your sneak key.", IDR_SLOWLY_PNG, "SHIFT") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new SneakListener("Sneak", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Sneak");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<bool>("status") == nullptr) settings.addSetting("status", false);

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);

        if (settings.getSettingByName<bool>("always") == nullptr) {
            settings.addSetting("always", false);
        }
    }

    void settingsRender() override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Keybind", "", getKeybind());

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};

