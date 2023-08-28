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

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"HotBar", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);

        float percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("HotBar "),
                                           toggleY, this->settings.getSettingByName<float>("hotbarSpeed")->value, 50.0f,
                                           0, 0);

        this->settings.getSettingByName<float>("hotbarSpeed")->value = percent;

    }
};
