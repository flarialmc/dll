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
			if (showChunkMap != nullptr) showChunkMap->setvalue(getOps<bool>("chunkborders"));
			if (disableSky != nullptr) disableSky->setvalue(!getOps<bool>("sky"));
			if (disableWeather != nullptr) disableWeather->setvalue(!getOps<bool>("weather"));
			if (disableEntities != nullptr) disableEntities->setvalue(!getOps<bool>("entity"));
			if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(!getOps<bool>("blockentity"));
			if (disableParticles != nullptr) disableParticles->setvalue(!getOps<bool>("particles"));
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
		setDef("chunkborders", false);
		setDef("sky", true);
		setDef("weather", true);
		setDef("entity", true);
		setDef("blockentity", true);
		setDef("particles", true);
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
		addToggle("Chunk Borders", "", getOps<bool>("chunkborders"));
		addToggle("Render Sky", "", getOps<bool>("sky"));
		addToggle("Render Entities", "", getOps<bool>("entity"));
		addToggle("Render Block Entities", "", getOps<bool>("blockentity"));
		addToggle("Render Particles", "", getOps<bool>("particles"));
		addToggle("Render Weather", "", getOps<bool>("weather"));

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (Options::isInitialized()) {
			updateSetings();
		};
	}
};