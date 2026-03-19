#pragma once

#include "../HUDModule.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"

class FPS : public HUDModule {

public:

	FPS() : HUDModule(0, "FPS", "Shows how much Frames Per Second (FPS)\nyour device is rendering.",
		IDR_FPS_PNG, "", {"stats"}) {
	};

	void onRenderUnderUI(RenderUnderUIEvent& event);

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customSettings() override;

	void customInit() override;

	void customCleanup() override;
};
