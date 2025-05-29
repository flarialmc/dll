#pragma once

#include "../Module.hpp"

class ComboCounter : public Module {
private:
	int Combo = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
public:

	ComboCounter() : Module("Combo", "Keeps track of consecutive hits.", IDR_COMBO_PNG, "") {

		Module::setup();

	};

	void onEnable() override {
		Listen(this, AttackEvent, &ComboCounter::onAttack)
			Listen(this, TickEvent, &ComboCounter::onTick)
			Listen(this, RenderEvent, &ComboCounter::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, AttackEvent, &ComboCounter::onAttack)
			Deafen(this, TickEvent, &ComboCounter::onTick)
			Deafen(this, RenderEvent, &ComboCounter::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"Combo: {value}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
	}

	void settingsRender(float settingsOffset) override {

		/* Border Start */

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Main");
		this->defaultAddSettings("main");
		this->extraPadding();

		this->addHeader("Text");
		this->defaultAddSettings("text");
		this->extraPadding();

		this->addHeader("Colors");
		this->defaultAddSettings("colors");

		this->addHeader("Misc");
		this->defaultAddSettings("misc");
		
		FlarialGUI::UnsetScrollView();

		this->resetPadding();

	}

	void onAttack(AttackEvent& event) {
		if (std::chrono::high_resolution_clock::now() - last_hit > std::chrono::milliseconds(480)) {
			Combo++;
			last_hit = std::chrono::high_resolution_clock::now();
		}
	}

	void onTick(TickEvent& event) {
		if (!SDK::clientInstance->getLocalPlayer())
			return;

		auto LP = reinterpret_cast<LocalPlayer*>(event.getActor());
		if (LP->getHurtTime() != 0)
			Combo = 0;
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
		if (duration.count() >= 15) Combo = 0;
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			auto comboStr = FlarialGUI::cached_to_string(Combo);
			this->normalRender(8, comboStr);
		}
	}
};