#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"

class Waila : public Module {
public:
	std::string lookingAt;

	Waila() : Module("Waila", "Shows what you are looking at.", IDR_WAILA_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
			Listen(this, RenderEvent, &Waila::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
			Deafen(this, RenderEvent, &Waila::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<bool>("responsivewidth") == nullptr) settings.addSetting("responsivewidth", true);
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		Module::defaultConfig("all");
		if (settings.getSettingByName<bool>("advanced") == nullptr) settings.addSetting("advanced", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Main");
		defaultAddSettings("main");
		addToggle("Advanced Mode", "", settings.getSettingByName<bool>("advanced")->value);
		extraPadding();

		addHeader("Text");
		defaultAddSettings("text");
		extraPadding();

		addHeader("Colors");
		defaultAddSettings("colors");
		extraPadding();

		addHeader("Misc Customizations");
		defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}


	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (!SDK::clientInstance->getLocalPlayer()) return;
		if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
		if (!SDK::clientInstance->getBlockSource()) return;
		HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

		BlockPos pos = { result.blockPos.x,
						 result.blockPos.y ,
						 result.blockPos.z };
		BlockSource* blockSource = SDK::clientInstance->getBlockSource();
		try {
			BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
			if (!block) return;
			try {

				if (!this->settings.getSettingByName<bool>("advanced")->value) lookingAt = block->getName();
				else lookingAt = block->getNamespace() + ":" + block->getName();
			}
			catch (const std::exception& e) { LOG_ERROR("Failed to get block name: {}", e.what()); }
		}
		catch (const std::exception& e) {
			LOG_ERROR("Failed to get block: {}", e.what());
		}
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled() && lookingAt != "air") {
			this->normalRender(32, lookingAt);
		}
	}
};
