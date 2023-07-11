#pragma once

#include "../Module.hpp"
#include "MotionBlurListener.hpp"
#include "../../../Events/EventHandler.hpp"


class MotionBlur : public Module {

public:

    MotionBlur() : Module("MotionBlur", "chike wi!", "\\Flarial\\assets\\nametag.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new MotionBlurListener("MotionBlurListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {

        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.7f);

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, 40, 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(x, y, L"Intensity", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(7, x + Constraints::SpacingConstraint(0.65, textWidth),
            y,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("intensity")->value, 1.0f);

        this->settings.getSettingByName<float>("intensity")->value = percent;

    }
};