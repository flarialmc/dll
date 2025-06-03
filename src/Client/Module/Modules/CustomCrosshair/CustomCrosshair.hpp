#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../Events/Game/TickEvent.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Render/HudCursorRendererRenderEvent.hpp"


class CustomCrosshair : public Module {
private:
	Perspective currentPerspective;
public:
	CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
		IDR_SPEED_PNG, "") {

		Module::setup();
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetViewPerspective(PerspectiveEvent& event);

	void onHudCursorRendererRender(HudCursorRendererRenderEvent& event);
};
