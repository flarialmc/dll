#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:

    void NormalRender(int index, std::string text, std::string value) override {

    }

    Deepfry() : Module("Deepfry", "chike wi!", "\\Flarial\\assets\\frying-pan.png", 'b') {

        onEnable();

    };

    void onEnable() override {

        if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);

        EventHandler::registerListener(new DeepfryListener("Deepfry", this));
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"Paint Effect", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(1, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("paint")->value)) this->settings.getSettingByName<bool>("paint")->value = !this->settings.getSettingByName<bool>("paint")->value;


    }
};