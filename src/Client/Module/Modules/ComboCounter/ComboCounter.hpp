#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/AttackEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class ComboCounter : public HUDModule {
private:
	int lastHurtTime;
	int Combo = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
public:

	ComboCounter() : HUDModule(8, "Combo", "Keeps track of consecutive hits.",
		IDR_COMBO_PNG, "") {

	};

	void onAttack(AttackEvent& event);

	void onTick(TickEvent& event);

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customSettings() override;

	void customInit() override;

	void customCleanup() override;
};
