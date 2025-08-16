#pragma once

#include "../Module.hpp"
#include "Events/Game/AttackEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class ComboCounter : public Module {
private:
	int lastHurtTime;
	int Combo = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
public:

	ComboCounter() : Module("Combo", "Keeps track of consecutive hits.", IDR_COMBO_PNG, "") {

		

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onAttack(AttackEvent& event);

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
