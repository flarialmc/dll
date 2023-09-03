#pragma once

#include "HueListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class HueChanger : public Module {

public:

    void NormalRender(int index, std::string text, std::string value) override {

    }

    HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.", "\\Flarial\\assets\\fullbright.png", 'b') {

        onEnable();

    };

    void onEnable() override {

        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.5f);

        EventHandler::registerListener(new HueListener("Hue", this));
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.26, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(x, y, L"Saturation Intensity", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, x + FlarialGUI::SettingsTextWidth("Saturation Intensity "),
                                           y,
                                           this->settings.getSettingByName<float>("intensity")->value, 3.0f);

        this->settings.getSettingByName<float>("intensity")->value = percent;


    }
};