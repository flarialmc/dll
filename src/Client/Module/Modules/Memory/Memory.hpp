#pragma once

#include "../Module.hpp"



class MEM : public Module {

public:

	MEM(): Module("Memory", "Shows your current system RAM usage.",
		IDR_MEMORY_PNG, "", false, {"stats"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};