#pragma once

#include "../Module.hpp"
#include "GuiScaleListener.hpp"
#include "../../../Events/EventHandler.hpp"


class GuiScale : public Module {

public:

    GuiScale() : Module("MC GUI Scale", "chike wi!", "\\Flarial\\assets\\nametag.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        std::vector<uint8_t> original;
        original.resize(4);
        memcpy(original.data(), (LPVOID)Memory::findSig("00 00 ? ? 00 00 A0 40 00 00 C0 40"), 4);

        EventHandler::registerListener(new GuiScaleListener("GuiScaleListener", this, original));

        if(settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);


        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {

        if(settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);

    }

    void onDisable() override {
        GuiScaleListener::unpatch();
        Module::onDisable();
    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));


        float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("guiscale")->value, 4.0f);

        this->settings.getSettingByName<float>("guiscale")->value = percent;

    }
};