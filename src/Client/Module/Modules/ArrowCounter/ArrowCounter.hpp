#pragma once

#include "../Module.hpp"

class ArrowCounter : public Module {
private:
	int arrows = 0;
public:

	ArrowCounter() : Module("Arrow Counter", "Counts how many arrows you have\nin your inventory.",
		IDR_ARROW_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, TickEvent, &ArrowCounter::onTick)
			Listen(this, RenderEvent, &ArrowCounter::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, TickEvent, &ArrowCounter::onTick)
			Deafen(this, RenderEvent, &ArrowCounter::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value} Arrows");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
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
		this->defaultAddSettings("main");
		this->extraPadding();

		this->addHeader("Text");
		this->defaultAddSettings("text");
		this->extraPadding();

		this->addHeader("Colors");
		this->defaultAddSettings("colors");
		this->extraPadding();

		this->addHeader("Misc");
		this->defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}


	void onTick(TickEvent& event) {
		if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
					if (SDK::getCurrentScreen() != "hud_screen") return;
					auto arrowsCount = 0;

					auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();
					if (inventory == nullptr) return;

					auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();
					if (offhandItem != nullptr)
						if (offhandItem->getItem() != nullptr)
							if (offhandItem->getItem()->name == "arrow")
								arrowsCount = offhandItem->count;


					for (int i = 0; i < 36; i++) {
						auto item = inventory->getItem(i);

						if (item->getItem() != nullptr) {
							if (item->getItem()->name == "arrow") {
								arrowsCount += item->count;
							}

						}
					}

					arrows = arrowsCount;
				}
			}
		}
	}

	void onRender(RenderEvent& event) {
		auto arrowsStr = FlarialGUI::cached_to_string(arrows);
		this->normalRender(13, arrowsStr);
	}
};