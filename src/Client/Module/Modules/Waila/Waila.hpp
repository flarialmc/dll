#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class Waila : public Module {
public:
	std::string lookingAt;

	Waila() : Module("Waila", "Shows what you are looking at.", IDR_WAILA_PNG, "") {
		//Module::setup();
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;


	void onSetupAndRender(SetupAndRenderEvent& event);

	void onRender(RenderEvent& event);
};
