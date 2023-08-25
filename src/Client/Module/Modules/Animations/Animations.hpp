#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Animations : public Module {

public:

    Animations() : Module("Animations", "chike wi!", "\\Flarial\\assets\\Animations.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        if (settings.getSettingByName<float>("hotbarSpeed") == nullptr) settings.addSetting("hotbarSpeed", 7.0f);


        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {



    }

    void onDisable() override {

        Module::onDisable();
    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"HotBar", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));


        float percent = FlarialGUI::Slider(1, toggleX + Constraints::SpacingConstraint(0.60, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("hotbarSpeed")->value, 15.0f);

        this->settings.getSettingByName<float>("hotbarSpeed")->value = percent;

    }
};
