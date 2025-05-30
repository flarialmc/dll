#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"


class Zoom : public Module {
private:
	static inline float currentZoomVal = 0.0f;
	static float inline zoomValue = 30.0f;
	static float inline realFov = 70.0f;
	static inline float currentSensitivity = 0.0f;
	static inline bool saved = false;

	static inline float unzoomAnimDisableTreshold = 0.2;

	static inline bool fisrtTime = true;

public:
	static inline bool animationFinished = true;
	static inline bool jdfAnimationFinished = true;
	Zoom() : Module("Zoom", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, FOVEvent, &Zoom::onGetFOV)
			Listen(this, SensitivityEvent, &Zoom::onGetSensitivity)
			Listen(this, MouseEvent, &Zoom::onMouse)
			Listen(this, KeyEvent, &Zoom::onKey)
			Listen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, FOVEvent, &Zoom::onGetFOV)
			Deafen(this, SensitivityEvent, &Zoom::onGetSensitivity)
			Deafen(this, MouseEvent, &Zoom::onMouse)
			Deafen(this, KeyEvent, &Zoom::onKey)
			Deafen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<bool>("enabled") == nullptr) settings.addSetting("enabled", true);
		Module::defaultConfig("core");
		if (settings.getSettingByName<bool>("alwaysanim") == nullptr) settings.addSetting("alwaysanim", false);
		if (settings.getSettingByName<bool>("SaveModifier") == nullptr) settings.addSetting("SaveModifier", true);
		if (settings.getSettingByName<bool>("hidehand") == nullptr) settings.addSetting("hidehand", true);
		if (settings.getSettingByName<bool>("hidemodules") == nullptr) settings.addSetting("hidemodules", false);
		if (settings.getSettingByName<bool>("UseScroll") == nullptr) settings.addSetting("UseScroll", true);
		if (settings.getSettingByName<bool>("lowsens") == nullptr) settings.addSetting("lowsens", true);
		if (settings.getSettingByName<bool>("toggleZoom") == nullptr) settings.addSetting("toggleZoom", false);
		//if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
		if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
		if (settings.getSettingByName<float>("anim") == nullptr) settings.addSetting("anim", 0.20f);
		if (settings.getSettingByName<float>("disableanim") == nullptr) settings.addSetting("disableanim", false);
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
		addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
		addToggle("Use Scroll", "Allows to adjust zoom with scroll wheel.", settings.getSettingByName<bool>("UseScroll")->value);
		addToggle("Toggle Zoom", "No need to hold the keybind to zoom.", settings.getSettingByName<bool>("toggleZoom")->value);
		addSlider("Modifier", "How much to Zoom each time you scroll.", settings.getSettingByName<float>("modifier")->value, 30, 0, false);
		addToggle("Disable Animation", "The animation when you zoom", settings.getSettingByName<bool>("disableanim")->value);
		addSlider("Animation Speed", "Speed at which scroll zoom acts.", settings.getSettingByName<float>("anim")->value, 0.40);
		addToggle("Save Modifier", "Saves the last zoom amount.", settings.getSettingByName<bool>("SaveModifier")->value);
		addToggle("Hide Hand", "Hide hand when zooming.", settings.getSettingByName<bool>("hidehand")->value);
		addToggle("Hide Modules", "Hides other modules when zooming.", settings.getSettingByName<bool>("hidemodules")->value);
		addToggle("Always Animate", "Smooth zoom animation while sprinting.", settings.getSettingByName<bool>("alwaysanim")->value);
		addToggle("Low Sensitivity", "Lower sensitivity when in zoom.", settings.getSettingByName<bool>("lowsens")->value);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onGetFOV(FOVEvent& event) {
		auto fov = event.getFOV();
		if (fov == 70) return;

		auto player = SDK::clientInstance->getLocalPlayer();
		if (!player) return;

		if (ModuleManager::getModule("Java Dynamic FOV").get()->isEnabled()) {
			if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
				fov = ModuleManager::getModule("Java Dynamic FOV").get()->settings.getSettingByName<float>("fov_target")->value;
			}
		}

		if (fisrtTime) { // so that it doesn't unzoom on module load
			currentZoomVal = fov;
			fisrtTime = false;
			return;
		}

		float animspeed = this->settings.getSettingByName<float>("anim")->value;
		bool disableanim = this->settings.getSettingByName<bool>("disableanim")->value;
		bool alwaysanim = this->settings.getSettingByName<bool>("alwaysanim")->value;

		if (this->active) {
			animationFinished = false;
			if (fov > 180) {
				currentZoomVal = disableanim ? fov + zoomValue : std::lerp(currentZoomVal, fov + zoomValue, animspeed * FlarialGUI::frameFactor);
			}
			else {
				currentZoomVal = disableanim ? zoomValue : std::lerp(currentZoomVal, zoomValue, animspeed * FlarialGUI::frameFactor);
			}
		}
		else {
			if ((!animationFinished || alwaysanim) && !disableanim) {
				// Only lerp if animation hasn't finished
				if (!jdfAnimationFinished)
				{
					jdfAnimationFinished = true;
				}
				currentZoomVal = std::lerp(currentZoomVal, fov, animspeed * FlarialGUI::frameFactor);
				if (currentZoomVal == zoomValue || std::abs(fov - currentZoomVal) < animspeed + unzoomAnimDisableTreshold) { // when fov changes due to sprinting animation used to play
					// Set animationFinished to true only when reaching original fov
					animationFinished = true;
				}
			}
			else {
				// Once animation finished, set current zoom to fov
				currentZoomVal = fov;
			}
		}

		event.setFOV(currentZoomVal);
	}

	void onGetSensitivity(SensitivityEvent& event) {
		if (this->active) {
			if (!saved) {
				saved = true;
				currentSensitivity = event.getSensitivity();
			}
			// TODO: smoothstep
			if (this->settings.getSettingByName<bool>("lowsens")->value) event.setSensitivity(currentSensitivity - (currentSensitivity * (((realFov - (zoomValue - 1)) / realFov) / 1.0f)));
		}
		else if (saved) {
			saved = false;
		}
	}

	//TODO: RE CHECK
	void onMouse(MouseEvent& event) {
		if (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f1_screen" || SDK::getCurrentScreen() == "zoom_screen")
			if (this->active) {
				//todo make it so that modules work together
				auto fovchanger = ModuleManager::getModule("FOV Changer");
				auto upsidedown = ModuleManager::getModule("Upside Down");
				if (this->settings.getSettingByName<bool>("UseScroll")->value == true) {
					if (event.getAction() == MouseAction::ScrollUp) {
						if ((fovchanger != nullptr &&
							fovchanger->settings.getSettingByName<float>("fovvalue")->value > 180) ||
							(upsidedown != nullptr && upsidedown->isEnabled()))
							zoomValue += this->settings.getSettingByName<float>("modifier")->value;
						else zoomValue -= this->settings.getSettingByName<float>("modifier")->value;
					}
					if (event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
						if ((fovchanger != nullptr &&
							fovchanger->settings.getSettingByName<float>("fovvalue")->value > 180) ||
							(upsidedown != nullptr && upsidedown->isEnabled()))
							zoomValue -= this->settings.getSettingByName<float>("modifier")->value;
						else zoomValue += this->settings.getSettingByName<float>("modifier")->value;
					}

					if (zoomValue < 1) zoomValue = 1;
					else if (zoomValue > realFov) zoomValue = realFov;

					if (event.getAction() == MouseAction::ScrollUp ||
						event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
						event.setButton(MouseButton::None);
						event.setAction(MouseAction::Release);


					}
				}
			}
	}

	void onKey(KeyEvent& event) {
		
		if (this->isKeybind(event.keys) && 
			this->isKeyPartOfKeybind(event.key) && 
			(this->settings.getSettingByName<bool>("toggleZoom")->value ? event.getAction() == ActionType::Pressed : true)
			&& SDK::getCurrentScreen() == "hud_screen") {
			keybindActions[0]({});
			if (!this->settings.getSettingByName<bool>("SaveModifier")->value) zoomValue = 30.0f;
		} else if (this->settings.getSettingByName<bool>("toggleZoom")->value ? (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) : !this->isKeybind(event.keys) && (this->settings.getSettingByName<bool>("toggleZoom")->value ? event.getAction() == ActionType::Pressed : true)) this->active = false;
	};

	// TODO: add dolly zoom and world2screen zoom stabilization ?
	// TODO: if someone has disabled hand this will enable it unless changed in settings
	void onSetTopScreenName(SetTopScreenNameEvent& event) {
		auto hideHand = Options::getOption("hidehand");
		auto hideHud = Options::getOption("hidehud");
		if (this->active) {
			if (this->settings.getSettingByName<bool>("hidemodules")->value) {
				event.setCustomLayer("zoom_screen");
			}
			//            if (this->settings.getSettingByName<bool>("hidehud")->value) {
			//                event.setCustomLayer("zoom_screen");
			//                if (hideHand != nullptr) hideHand->setvalue(true);
			//                if (hideHud != nullptr) hideHud->setvalue(true);
			//            }
			if (this->settings.getSettingByName<bool>("hidehand")->value) {
				if (hideHand != nullptr) hideHand->setvalue(true);
			}
		}
		else {
			if (this->settings.getSettingByName<bool>("hidehand")->value) {
				if (hideHand != nullptr) hideHand->setvalue(false);
			}

			//            if (this->settings.getSettingByName<bool>("hidehud")->value) { // TODO: there is a bug where it wont allow manual F1
			//                if (hideHud != nullptr) hideHud->setvalue(false);
			//            }

			if (hideHud != nullptr && hideHud->getvalue()) {
				event.setCustomLayer("f1_screen");
			}
		}
	}
};
