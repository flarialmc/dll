#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FullbrightListener.hpp"


class Fullbright : public Module {

public:

    Fullbright() : Module("Fullbright",
                          "No need for torches!\nProvides consistent and constant illumination.\nEffectively removing darkness and shadows.",
                          IDR_FULLBRIGHT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new FullbrightListener("Fullbright", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Fullbright");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("gamma") == nullptr)
            settings.addSetting("gamma", 25.0f);
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

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Gamma", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("Gamma"),
                                           toggleY, this->settings.getSettingByName<float>("gamma")->value, 25.0f, // 3.3
                                           0.0f);

        this->settings.getSettingByName<float>("gamma")->value = percent;

        FlarialGUI::UnsetScrollView();


    }
};