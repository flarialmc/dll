#pragma once
#include "../../../Events/Events.hpp"
#include "../Module.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"


class Zoom : public Module {
private:
	static inline float currentZoomVal = 0.0f;
	static float inline zoomValue = 30.0f;
	static float inline realFov = 70.0f;
	static inline float currentSensitivity = 0.0f;
	static inline bool saved = false;
	static inline float currentFov = 70.0f;

	static inline float unzoomAnimDisableTreshold = 0.2;

	static inline bool fisrtTime = true;

	Vec2<float> smoothDelta{0, 0};
	std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();

public:
	static inline bool animationFinished = true;
	static inline bool jdfAnimationFinished = true;
	Zoom();


	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetFOV(FOVEvent& event);

	void checkKeybind(int key);

	void onRender(RenderEvent& event);

	//TODO: RE CHECK
	void onMouse(MouseEvent& event);

	void onKey(KeyEvent& event);

	// TODO: add dolly zoom and world2screen zoom stabilization ?
	// TODO: if someone has disabled hand this will enable it unless changed in settings
	void onSetTopScreenName(SetTopScreenNameEvent& event);
	void onTurnDeltaEvent(TurnDeltaEvent& event);
};