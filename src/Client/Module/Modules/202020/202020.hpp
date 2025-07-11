#pragma once

#include "../Module.hpp"

class Module202020 : public Module {
public:
	std::chrono::steady_clock::time_point now;
	std::chrono::steady_clock::time_point last;
	std::chrono::steady_clock::time_point blackscreen;
	std::chrono::seconds elapsed;

	Module202020() : Module("Meds", "Implements the medical 202020 rule\ninto minecraft.", IDR_TIME_PNG, "") {
		
	};

	void onEnable() override {
		now = std::chrono::steady_clock::now();
		last = now;
		Listen(this, RenderEvent, &Module202020::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Module202020::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		setDef("extreme", false);
		
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Main");
		this->addToggle("Extreme Mode", "", "extreme");

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void onRender(RenderEvent& event) {
		if (!this->isEnabled()) return;
		now = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last);

		if (elapsed >= std::chrono::minutes(20)) {
			if (!getOps<bool>("extreme")) FlarialGUI::Notify("Look at something 20 feet away for 20 seconds!");
			last = now;
			blackscreen = now;
		}

		if (getOps<bool>("extreme") and std::chrono::duration_cast<std::chrono::seconds>(now - last) < std::chrono::seconds(20) and elapsed >= std::chrono::minutes(20)) {
			FlarialGUI::RoundedRect(0, 0, { 0.f, 0.f, 0.f, 1.f }, Constraints::PercentageConstraint(1, "left"), Constraints::PercentageConstraint(1, "top"), 0, 0);
		}
	}
};