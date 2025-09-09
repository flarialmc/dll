#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"
#include "../Zoom/Zoom.hpp"

class JavaDynamicFOV : public Module {
private:
	static inline float currentFOVVal = 0.0f;
	static float inline realFov = 70.0f;

	static inline float animDisableTreshold = 0.2;

	static inline bool firstTime = true;

public:
	static inline bool animationFinished = true;
	JavaDynamicFOV() : Module("Java Dynamic FOV", "Enhances dynamic FOV in bedrock.",
		IDR_MAGNIFY_PNG, "C") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetFOV(FOVEvent& event);
};
