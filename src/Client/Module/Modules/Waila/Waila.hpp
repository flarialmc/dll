#pragma once

#include "../HUDModule.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class Waila : public HUDModule {
public:
	std::string lookingAt;

	Waila() : HUDModule(32, "Waila", "Shows what you are looking at.",
		IDR_WAILA_PNG, "") {
	};

	void onEnable() override;
	void onDisable() override;
	void onSetupAndRender(SetupAndRenderEvent& event);

protected:
	std::string getDisplayValue() override;
	void customConfig() override;
	void customSettings() override;
};
