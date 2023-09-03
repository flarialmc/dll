#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ArmorHUDListener.hpp"

class ArmorHUD : public Module {

public:

    ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.", "\\Flarial\\assets\\chestplate.png", 'Y') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();
        EventHandler::registerListener(new ArmorHUDListener("ArmorHUD", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("ArmorHUD");

        Module::onDisable();

    }

    void DefaultConfig() override {

        if (settings.getSettingByName<bool>("enabled") == nullptr) settings.addSetting("enabled", false);
        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, 40, 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        FlarialGUI::to_wide("Vertical").c_str(), textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(2, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "vertical")->value)) this->settings.getSettingByName<bool>("vertical")->value = !this->settings.getSettingByName<bool>("vertical")->value;


        FlarialGUI::UnsetScrollView();
    }
};