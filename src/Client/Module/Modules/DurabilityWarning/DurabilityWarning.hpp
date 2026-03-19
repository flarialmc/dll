#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"

class DurabilityWarning : public Module {
	bool notified = false;

public:
	DurabilityWarning() : Module("Durability Warning", "Notifies you when a tool reaches low durability.",
		IDR_BLOCK_BREAK_INDICATOR_PNG, "") {}

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onEnable() override;

	void onDisable() override;

	void onTick(TickEvent& event);
};
