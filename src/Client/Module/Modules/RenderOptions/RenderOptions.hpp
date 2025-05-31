#pragma once

#include "../Module.hpp"

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

		if (isEnabled()) {
			if (showChunkMap != nullptr) showChunkMap->setvalue(settings.getSettingByName<bool>("chunkborders")->value);
			if (disableSky != nullptr) disableSky->setvalue(!settings.getSettingByName<bool>("sky")->value);
			if (disableWeather != nullptr) disableWeather->setvalue(!settings.getSettingByName<bool>("weather")->value);
			if (disableEntities != nullptr) disableEntities->setvalue(!settings.getSettingByName<bool>("entity")->value);
			if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(!settings.getSettingByName<bool>("blockentity")->value);
			if (disableParticles != nullptr) disableParticles->setvalue(!settings.getSettingByName<bool>("particles")->value);
		}
		else {
			if (showChunkMap != nullptr) showChunkMap->setvalue(false);
			if (disableSky != nullptr) disableSky->setvalue(false);
			if (disableWeather != nullptr) disableWeather->setvalue(false);
			if (disableEntities != nullptr) disableEntities->setvalue(false);
			if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(false);
			if (disableParticles != nullptr) disableParticles->setvalue(false);
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

	void defaultConfig() override {
		Module::defaultConfig("core");
		if (settings.getSettingByName<bool>("chunkborders") == nullptr) settings.addSetting("chunkborders", false);
		if (settings.getSettingByName<bool>("sky") == nullptr) settings.addSetting("sky", true);
		if (settings.getSettingByName<bool>("weather") == nullptr) settings.addSetting("weather", true);
		if (settings.getSettingByName<bool>("entity") == nullptr) settings.addSetting("entity", true);
		if (settings.getSettingByName<bool>("blockentity") == nullptr) settings.addSetting("blockentity", true);
		if (settings.getSettingByName<bool>("particles") == nullptr) settings.addSetting("particles", true);
	}

	void settingsRender(float settingsOffset) override {

		updateSetings();

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Render Options");
		addToggle("Chunk Borders", "", settings.getSettingByName<bool>("chunkborders")->value);
		addToggle("Render Sky", "", settings.getSettingByName<bool>("sky")->value);
		addToggle("Render Entities", "", settings.getSettingByName<bool>("entity")->value);
		addToggle("Render Block Entities", "", settings.getSettingByName<bool>("blockentity")->value);
		addToggle("Render Particles", "", settings.getSettingByName<bool>("particles")->value);
		addToggle("Render Weather", "", settings.getSettingByName<bool>("weather")->value);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (Options::isInitialized()) {
			updateSetings();
		};
	}
};