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
		Module::defaultConfig();
		if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
	}

	void settingsRender() override {
		float toggleX = Constraints::PercentageConstraint(0.019, "left");
		float toggleY = Constraints::PercentageConstraint(0.10, "top");

		float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
		const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);


		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Modifier", textWidth * 3.0f, textHeight,
			DWRITE_TEXT_ALIGNMENT_LEADING,
			Constraints::RelativeConstraint(0.12, "height", true),
			DWRITE_FONT_WEIGHT_NORMAL);


		float percent = FlarialGUI::Slider(7, toggleX + FlarialGUI::SettingsTextWidth("Modifier "),
			toggleY, this->settings.getSettingByName<float>("modifier")->value, 360, 0,
			false);

		this->settings.getSettingByName<float>("modifier")->value = percent;

	}
};

