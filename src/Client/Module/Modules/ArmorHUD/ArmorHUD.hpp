#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ArmorHUDListener.hpp"

class ArmorHUD : public Module {

public:

    ArmorHUD() : Module("ArmorHUD", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\chestplate.png", 'Y') {

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

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(3, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                           D2D1::ColorF(D2D1::ColorF::White),
                                           this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;


        FlarialGUI::UnsetScrollView();
    }
};