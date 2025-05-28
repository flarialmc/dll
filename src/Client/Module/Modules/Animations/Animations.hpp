#pragma once

#include "../Module.hpp"


class Animations : public Module {
private:
	BedrockTextureData* selectedHotbarSlotTexturePtr = nullptr;
public:

	Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
		IDR_ANIMATIONS_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, DrawImageEvent, &Animations::onDrawImage)
			Listen(this, GetTextureEvent, &Animations::onGetTexture)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, DrawImageEvent, &Animations::onDrawImage)
			Deafen(this, GetTextureEvent, &Animations::onGetTexture)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();

		if (settings.getSettingByName<float>("hotbarSpeed") == nullptr) settings.addSetting("hotbarSpeed", 7.0f);

	}

	inline static float animate(float endPoint, float current, float speed) {
		if (speed < 0.0) speed = 0.0;
		else if (speed > 1.0) speed = 1.0;

		float dif = (((endPoint) > (current)) ? (endPoint) : (current)) - (((endPoint) < (current)) ? (endPoint) : (current));
		float factor = dif * speed;
		return current + (endPoint > current ? factor : -factor);
	}

	void onGetTexture(GetTextureEvent& event) {
		if (event.location->filePath == "textures/ui/selected_hotbar_slot") {
			selectedHotbarSlotTexturePtr = event.textureData;
		}
	}

	void onDrawImage(DrawImageEvent& event) {
		bool shouldAnimate = false;
		if (VersionUtils::checkAboveOrEqual(21, 50)) {
			if (selectedHotbarSlotTexturePtr == event.getTextureData()) {
				shouldAnimate = true;
			}
		}
		else {
			if (event.getTexturePath() == "textures/ui/selected_hotbar_slot") {
				shouldAnimate = true;
			}
		}

		if (shouldAnimate) {
			auto pos = event.getImagePos();
			static float lerpedPos = pos.x; // old pos
			lerpedPos = animate(pos.x, lerpedPos,
				(0.016f * this->settings.getSettingByName<float>("hotbarSpeed")->value) *
				FlarialGUI::frameFactor);
			event.setImagePos(Vec2<float>{lerpedPos, pos.y});
		}
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Misc");
		this->addSlider("Hotbar Selected Slot Speed", "", this->settings.getSettingByName<float>("hotbarSpeed")->value);

		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}
};
