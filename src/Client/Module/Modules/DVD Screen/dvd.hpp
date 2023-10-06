#pragma once

#include "../Module.hpp"
#include "dvdListener.hpp"
#include "../../../Events/EventHandler.hpp"


class DVD : public Module {

public:

    DVD() : Module("DVD Screen", "Overlays the DVD Screensaver", "\\Flarial\\assets\\skull.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new dvdListener("dvdListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {


        if (settings.getSettingByName<bool>("enabled") == nullptr)
            settings.addSetting("enabled", false);
        if (settings.getSettingByName<float>("xveloc") == nullptr)
            settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr)
            settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 1.0f);

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {


        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"X Velocity", textWidth * 3.0f, textHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::RelativeConstraint(0.12, "height", true),
            DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(0, toggleX + FlarialGUI::SettingsTextWidth("X Velocity "),
            toggleY, this->settings.getSettingByName<float>("xveloc")->value, 25.0f);

        this->settings.getSettingByName<float>("xveloc")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Y Velocity", textWidth * 3.0f, textHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::RelativeConstraint(0.12, "height", true),
            DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Y Velocity "),
            toggleY, this->settings.getSettingByName<float>("yveloc")->value, 25.0f);

        this->settings.getSettingByName<float>("yveloc")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Scale", textWidth * 3.0f, textHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            Constraints::RelativeConstraint(0.12, "height", true),
            DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(2, toggleX + FlarialGUI::SettingsTextWidth("Scale "),
            toggleY, this->settings.getSettingByName<float>("scale")->value, 5.0f);

        this->settings.getSettingByName<float>("scale")->value = percent;

        FlarialGUI::UnsetScrollView();

    }
};