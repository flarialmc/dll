#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"

class TotemCounter : public HUDModule {

private:
	int totems = 0;
    bool shouldRender = true;
    int lastTotemCount = 0;
    int tickCounter = 0;

public:
	TotemCounter() : HUDModule(35, "Totem Counter",
		"Counts how many totems you have\nin your inventory.",
		IDR_TOTEM_PNG, ""), totems(0), shouldRender(true), lastTotemCount(0), tickCounter(0) {
	};

	void onEnable() override;
	void onDisable() override;
	void onTick(TickEvent& event);

protected:
	std::string getDisplayValue() override;
	void customConfig() override;
	void customSettings() override;
};
