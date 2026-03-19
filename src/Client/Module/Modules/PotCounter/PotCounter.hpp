#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"

class PotCounter : public HUDModule {

private:
	int pots = 0;

public:
	PotCounter(): HUDModule(14, "Pot Counter", "Counts how much potions are\nin your inventory.",
		IDR_POTION_PNG, "", {"potion counter"}) {

	}

	void onTick(TickEvent& event);

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customInit() override;

	void customCleanup() override;
};
