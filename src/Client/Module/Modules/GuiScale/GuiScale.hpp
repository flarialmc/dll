#pragma once

#include "../Module.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class GuiScale : public Module {
private:
	float originalScale = 0.f;
	bool restored = false;
public:
	static inline bool fixResize = false;
	GuiScale() : Module("MC GUI Scale", "Change your GUI Scale beyond\nMinecraft's restrictions.",
		IDR_SCALE_PNG, "", false, {"size"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onSetupAndRender(SetupAndRenderEvent& event);;

	void update();

	void updateScale(float newScale);
};
