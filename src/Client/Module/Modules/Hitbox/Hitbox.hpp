#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "HitboxListener.hpp"

class Hitbox : public Module {

public:

    Hitbox() : Module("Hitbox", "Displays hitboxes of entities", IDR_BLOCK_PNG, "") {

        Module::setup();

    };

    // TODO: add options to see all entities

    void onEnable() override {
        EventHandler::registerListener(new HitboxListener("Hitbox", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Hitbox");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);
        if (settings.getSettingByName<float>("thickness") == nullptr) settings.addSetting("thickness", 1.1f);
        if (settings.getSettingByName<bool>("staticThickness") == nullptr) settings.addSetting("staticThickness", false);
        if (settings.getSettingByName<bool>("outline") == nullptr) settings.addSetting("outline", false);
    }

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Main");

        this->addToggle("2D Mode", "", this->settings.getSettingByName<bool>("outline")->value);
        this->addToggle("Static Thickness", "", this->settings.getSettingByName<bool>("staticThickness")->value);

        this->addSlider("Thickness", "", this->settings.getSettingByName<float>("thickness")->value);

        this->extraPadding();
        this->addHeader("Colors");
        this->addColorPicker("Hitbox Color", "", settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

};
