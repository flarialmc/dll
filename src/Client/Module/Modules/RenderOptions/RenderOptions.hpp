#pragma once

#include "../Module.hpp"

class RenderOptions : public Module {

public:

    RenderOptions() : Module("Render Option", "Change the way how the game is rendered.",
                             IDR_RENDEROPTIONS_PNG, "", false, false) {

        Module::setup();
    };

    void updateSetings() {
        auto showChunkMap = Options::getOption("dev_showChunkMap");
        auto disableSky = Options::getOption("dev_disableRenderSky");
        auto disableWeather = Options::getOption("dev_disableRenderWeather");
        auto disableEntities = Options::getOption("dev_disableRenderEntities");
        auto disableBlockEntities = Options::getOption("dev_disableRenderBlockEntities");
        auto disableParticles = Options::getOption("dev_renderBoundingBox");

        if(isEnabled()) {
            if(showChunkMap != nullptr) showChunkMap->setvalue(settings.getSettingByName<bool>("chunkborders")->value);
            if(disableSky != nullptr) disableSky->setvalue(!settings.getSettingByName<bool>("sky")->value);
            if(disableWeather != nullptr) disableWeather->setvalue(!settings.getSettingByName<bool>("weather")->value);
            if(disableEntities != nullptr) disableEntities->setvalue(!settings.getSettingByName<bool>("entity")->value);
            if(disableBlockEntities != nullptr) disableBlockEntities->setvalue(!settings.getSettingByName<bool>("blockentity")->value);
            if(disableParticles != nullptr) disableParticles->setvalue(!settings.getSettingByName<bool>("particles")->value);
        } else {
            if(showChunkMap != nullptr) showChunkMap->setvalue(false);
            if(disableSky != nullptr) disableSky->setvalue(false);
            if(disableWeather != nullptr) disableWeather->setvalue(false);
            if(disableEntities != nullptr) disableEntities->setvalue(false);
            if(disableBlockEntities != nullptr) disableBlockEntities->setvalue(false);
            if(disableParticles != nullptr) disableParticles->setvalue(false);
        }
    }

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &RenderOptions::onSetupAndRender)
        Module::onEnable();
        updateSetings();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &RenderOptions::onSetupAndRender)
        Module::onDisable();
        updateSetings();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<bool>("chunkborders") == nullptr) settings.addSetting("chunkborders", false);
        if (settings.getSettingByName<bool>("sky") == nullptr) settings.addSetting("sky", true);
        if (settings.getSettingByName<bool>("weather") == nullptr) settings.addSetting("weather", true);
        if (settings.getSettingByName<bool>("entity") == nullptr) settings.addSetting("entity", true);
        if (settings.getSettingByName<bool>("blockentity") == nullptr) settings.addSetting("blockentity", true);
        if (settings.getSettingByName<bool>("particles") == nullptr) settings.addSetting("particles", true);
    }

    void settingsRender(float settingsOffset) override {

        updateSetings();

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Chunk Borders", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "chunkborders")->value))
            this->settings.getSettingByName<bool>("chunkborders")->value = !this->settings.getSettingByName<bool>(
                    "chunkborders")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Render Sky", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(2, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "sky")->value))
            this->settings.getSettingByName<bool>("sky")->value = !this->settings.getSettingByName<bool>("sky")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Render Entities", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(3, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "entity")->value))
            this->settings.getSettingByName<bool>("entity")->value = !this->settings.getSettingByName<bool>(
                    "entity")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Render Block Entities", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "blockentity")->value))
            this->settings.getSettingByName<bool>("blockentity")->value = !this->settings.getSettingByName<bool>(
                    "blockentity")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Render Particles", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(5, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "particles")->value))
            this->settings.getSettingByName<bool>("particles")->value = !this->settings.getSettingByName<bool>(
                    "particles")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Render Weather", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(6, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "weather")->value))
            this->settings.getSettingByName<bool>("weather")->value = !this->settings.getSettingByName<bool>(
                    "weather")->value;

        FlarialGUI::UnsetScrollView();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        if(Options::isInitialized()) {
            updateSetings();
        };
    }
};