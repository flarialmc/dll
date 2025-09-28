#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"

class TestModule : public Module {

public:

	TestModule() : Module("TestModule", "Test module for debugging purposes.",
		0, "", false, {"test"}) {
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
	void onSetupAndRender(SetupAndRenderEvent& ev);

	void onRender(RenderEvent& event);
};