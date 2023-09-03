#pragma once

#include "../Module.hpp"
#include "MotionBlurListener.hpp"
#include "../../../Events/EventHandler.hpp"


class MotionBlur : public Module {

public:

    MotionBlur() : Module("Motion Blur", "Make fast movements appear smoother and more realistic by\nblurring the image slightly in the direction of motion.", "\\Flarial\\assets\\blur.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new MotionBlurListener("MotionBlurListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {

        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.88f);
        if (settings.getSettingByName<float>("intensity2") == nullptr) settings.addSetting("intensity2", 6.0f);
    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Bleed Factor", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);

        float percent = FlarialGUI::Slider(7, toggleX + FlarialGUI::SettingsTextWidth("Bleed Factor "),
                                           toggleY, this->settings.getSettingByName<float>("intensity")->value, 1.0f, 0,
                                           0);

        this->settings.getSettingByName<float>("intensity")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Intensity", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);


        percent = FlarialGUI::Slider(8, toggleX + FlarialGUI::SettingsTextWidth("Intensity "),
                                     toggleY, this->settings.getSettingByName<float>("intensity2")->value, 30);


        this->settings.getSettingByName<float>("intensity2")->value = percent;

        FlarialGUI::UnsetScrollView();

    }
};