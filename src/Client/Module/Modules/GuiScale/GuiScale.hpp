#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class GuiScale : public Module {

public:

    GuiScale() : Module("MC GUI Scale", "Change your GUI Scale beyond\nMinecraft's restrictions.", "\\Flarial\\assets\\nametag.png", 'C') {

        onEnable();

    };

    void onEnable() override {

                if(settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);


        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {



    }

    virtual void DefaultConfig() override {

        if(settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);

    }

    void onDisable() override {
        
        Module::onDisable();
    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY, this->settings.getSettingByName<float>("guiscale")->value, 4.0f, 0,
                                           0);

        this->settings.getSettingByName<float>("guiscale")->value = percent;

    }
};
