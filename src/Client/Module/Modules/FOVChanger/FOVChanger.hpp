#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"

#include "Assets/Assets.hpp"

class FOVChanger : public Module {
private:
	bool notified150Fov = false;
public:
	FOVChanger() : Module("FOV Changer", "Change your FOV beyond Minecraft's limits.",
		IDR_FIELD_OF_VIEW_PNG, "") {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetFOV(FOVEvent& event);

	void onRender(RenderEvent& event);

	void onRenderUnderUI(RenderUnderUIEvent& event);

private:
	void renderPanini(ID3D11RenderTargetView* rtv);
};
