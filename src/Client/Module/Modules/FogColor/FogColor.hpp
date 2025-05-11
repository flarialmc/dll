#pragma once

#include "../Module.hpp"


class FogColor : public Module {

public:

    FogColor() : Module("Fog Color", "Changes the color of the\nMinecraft fog.", IDR_SMOKE_PNG, "", false, false) {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, FogColorEvent, &FogColor::onGetFogColor)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, FogColorEvent, &FogColor::onGetFogColor)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);

    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Colors");
        this->addColorPicker("Fog Color", "", settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

    void onGetFogColor(FogColorEvent &event) {
        D2D1_COLOR_F color;
        if (this->settings.getSettingByName<bool>("color_rgb")->value)
            color = FlarialGUI::rgbColor;
        else
            color = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color")->value);

        event.setFogColorFromD2DColor(color, this->settings.getSettingByName<float>("colorOpacity")->value);
    }
};