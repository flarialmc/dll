#pragma once

#include "../Module.hpp"


class InventoryBlur : public Module {

public:

    InventoryBlur() : Module("Invetory Blur", "Blurs The Background when you open chest, inventory or any other ui.",
        IDR_FULLBRIGHT_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &InventoryBlur::onRender)
            Module::onEnable();
    }


    void onDisable() override {
        Deafen(this, RenderEvent, &InventoryBlur::onRender)
            Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig();
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 15);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addSlider("Intensity", "Intensity off the blur", settings.getSettingByName<float>("intensity")->value, 50);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    float intensity = 0;

    void onRender(RenderEvent& event) {
        if (this->isEnabled()) {
            if (SDK::clientInstance->getLocalPlayer() and SDK::currentScreen != "hud_screen") {
                FlarialGUI::lerp(intensity, settings.getSettingByName<float>("intensity")->value,
                    0.15f * FlarialGUI::frameFactor);
                if (intensity > 0.01)
                    Blur::RenderBlur(event.RTV, 3, intensity);
            }
        }
    }
};