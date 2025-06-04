#include "Animations.hpp"

#include "Client.hpp"
#include "Events/EventManager.hpp"

void Animations::onEnable() {
    Listen(this, DrawImageEvent, &Animations::onDrawImage)
			Listen(this, GetTextureEvent, &Animations::onGetTexture)
			Module::onEnable();
}

void Animations::onDisable() {
	Deafen(this, DrawImageEvent, &Animations::onDrawImage)
		Deafen(this, GetTextureEvent, &Animations::onGetTexture)
		Module::onDisable();
}

void Animations::onGetTexture(GetTextureEvent &event) {
	if (event.location->filePath == "textures/ui/selected_hotbar_slot") {
		selectedHotbarSlotTexturePtr = event.textureData;
	}
}

void Animations::onDrawImage(DrawImageEvent &event) {
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
			(0.016f * getOps<float>("hotbarSpeed")) *
			FlarialGUI::frameFactor);
		event.setImagePos(Vec2<float>{lerpedPos, pos.y});
	}
}

void Animations::defaultConfig() {
	Module::defaultConfig("core");
	setDef("hotbarSpeed", 7.f);
}

void Animations::settingsRender(float settingsOffset) {

	float x = Constraints::PercentageConstraint(0.019, "left");
	float y = Constraints::PercentageConstraint(0.10, "top");

	const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


	FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
	FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
		Constraints::RelativeConstraint(1.0, "width"),
		Constraints::RelativeConstraint(0.88f, "height"));

	addHeader("Animations");
	addSlider("Hotbar Selected Slot Speed", "", "hotbarSpeed");

	FlarialGUI::UnsetScrollView();

	resetPadding();
}
