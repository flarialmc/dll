#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SwingListener.hpp"


class Swing : public Module {

public:
	Swing() : Module("Swing", "Swing", IDR_LIKE_PNG, "") {
		Module::setup();
	};


	void onEnable() override {
		if (SwingListener::fluxSwingAddr == 0) SwingListener::fluxSwingAddr = (void*)Memory::findSig(GET_SIG("ItemInHandRenderer::fluxSwing"));
		if (SwingListener::fluxSwingAddr == 0) return;
		if (SwingListener::tapSwingVal == nullptr) {
			SwingListener::tapSwingVal = reinterpret_cast<float*>(Memory::offsetFromSig(Memory::findSig(GET_SIG("ItemInHandRenderer::TapSwingAnim")), 5));
			Memory::SetProtection(reinterpret_cast<uintptr_t>(SwingListener::tapSwingVal), 4, PAGE_READWRITE);
		}
		if (SwingListener::tapSwingVal == nullptr) return;
		SwingListener::patchAnimFunc(true);

		EventHandler::registerListener(new SwingListener("Swing", this));
		Module::onEnable();
	}

	void onDisable() override {
		std::vector<uint8_t> bytes = { 0x90, 0x90, 0x90, 0x90, 0x90 };

		Memory::patchBytes(SwingListener::fluxSwingAddr, bytes.data(), 5);
		*SwingListener::tapSwingVal = -80;


		SwingListener::patchAnimFunc(false);

		EventHandler::unregisterListener("Swing");
		Module::onDisable();

	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Toggle Sprint");
		addSlider("Modifier", "", settings.getSettingByName<float>("modifier")->value, 360.f, 0.f, false);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}
};

