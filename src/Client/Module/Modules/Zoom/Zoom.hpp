#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:

    Zoom() : Module("Zoom", "chike wi!", "\\Flarial\\assets\\magnify.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new ZoomListener("Zoom", this));

        Module::onEnable();

        if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"real");
        if(settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "C";
    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    
    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, 40, 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);

        y += Constraints::SpacingConstraint(0.35, textWidth);

        float percent = FlarialGUI::Slider(7, x,
            y,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("modifier")->value, 30);

        this->settings.getSettingByName<float>("modifier")->value = percent;

        FlarialGUI::UnsetScrollView();

    }
};