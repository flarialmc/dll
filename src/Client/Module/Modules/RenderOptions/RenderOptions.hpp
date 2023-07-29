#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class RenderOptions : public Module {

public:

    void NormalRender(int index, std::string text, std::string value) override {

    }

    RenderOptions() : Module("Render Options", "chike wi!", "\\Flarial\\assets\\renderoptions.png", 'b') {

        onEnable();

    };

    void onEnable() override {

        if (settings.getSettingByName<bool>("chunkborders") == nullptr) settings.addSetting("chunkborders", false);
        if (settings.getSettingByName<bool>("sky") == nullptr) settings.addSetting("sky", true);
        if (settings.getSettingByName<bool>("weather") == nullptr) settings.addSetting("weather", true);
        if (settings.getSettingByName<bool>("entity") == nullptr) settings.addSetting("entity", true);
        if (settings.getSettingByName<bool>("blockentity") == nullptr) settings.addSetting("blockentity", true);
        if (settings.getSettingByName<bool>("particles") == nullptr) settings.addSetting("particles", true);

        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.2, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));


        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Chunk Borders", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(1, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("chunkborders")->value)) this->settings.getSettingByName<bool>("chunkborders")->value = !this->settings.getSettingByName<bool>("chunkborders")->value;

        y += Constraints::SpacingConstraint(0.20, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Render Sky", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(2, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("sky")->value)) this->settings.getSettingByName<bool>("sky")->value = !this->settings.getSettingByName<bool>("sky")->value;

        y += Constraints::SpacingConstraint(0.20, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Render Entities", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(3, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("entity")->value)) this->settings.getSettingByName<bool>("entity")->value = !this->settings.getSettingByName<bool>("entity")->value;

        y += Constraints::SpacingConstraint(0.20, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Render Block Entities", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(4, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("blockentity")->value)) this->settings.getSettingByName<bool>("blockentity")->value = !this->settings.getSettingByName<bool>("blockentity")->value;

        y += Constraints::SpacingConstraint(0.20, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Render Particles", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(5, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("particles")->value)) this->settings.getSettingByName<bool>("particles")->value = !this->settings.getSettingByName<bool>("particles")->value;

        y += Constraints::SpacingConstraint(0.20, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.365, textWidth), y, L"Render Weather", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        if (FlarialGUI::Toggle(6, Constraints::PercentageConstraint(0.019, "left"), y, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("weather")->value)) this->settings.getSettingByName<bool>("weather")->value = !this->settings.getSettingByName<bool>("weather")->value;

        FlarialGUI::UnsetScrollView();
    }
};