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
	if (!this->isEnabled()) return;
	if (event.location->filePath == "textures/ui/selected_hotbar_slot") {
		selectedHotbarSlotTexturePtr = event.textureData;
	}
}

void Animations::onDrawImage(DrawImageEvent &event) {
	if (!this->isEnabled()) return;
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

	initSettingsPage();

	addHeader("Animations");
	addSlider("Hotbar Selected Slot Speed", "", "hotbarSpeed");

	FlarialGUI::UnsetScrollView();

	resetPadding();
}
