#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "RenderOptionsListener.hpp"


class RenderOptions : public Module {

public:

    RenderOptions() : Module("Render Option", "Change the way how the game is rendered.",
                             IDR_RENDEROPTIONS_PNG, "") {

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
        }else{
            if(showChunkMap != nullptr) showChunkMap->setvalue(false);
            if(disableSky != nullptr) disableSky->setvalue(false);
            if(disableWeather != nullptr) disableWeather->setvalue(false);
            if(disableEntities != nullptr) disableEntities->setvalue(false);
            if(disableBlockEntities != nullptr) disableBlockEntities->setvalue(false);
            if(disableParticles != nullptr) disableParticles->setvalue(false);
        }
    }

    void onEnable() override {
        Module::onEnable();
        if(!Options::isInitialized()) { // Wait for options to load and update them
            EventHandler::registerListener(new RenderOptionsListener("RenderOptions", this));
        } else {
            updateSetings();
        }
    }

    void onDisable() override {
        Module::onDisable();
        updateSetings();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("chunkborders") == nullptr) settings.addSetting("chunkborders", false);
        if (settings.getSettingByName<bool>("sky") == nullptr) settings.addSetting("sky", true);
        if (settings.getSettingByName<bool>("weather") == nullptr) settings.addSetting("weather", true);
        if (settings.getSettingByName<bool>("entity") == nullptr) settings.addSetting("entity", true);
        if (settings.getSettingByName<bool>("blockentity") == nullptr) settings.addSetting("blockentity", true);
        if (settings.getSettingByName<bool>("particles") == nullptr) settings.addSetting("particles", true);
    }

    void settingsRender() override { }
};