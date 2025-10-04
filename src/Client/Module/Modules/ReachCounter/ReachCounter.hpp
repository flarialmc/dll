#pragma once

#include "../Module.hpp"
#include "Events/Game/AttackEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class ReachCounter : public Module {

private:

	float Reach = 0.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;

public:

	ReachCounter(): Module("Reach Counter", "Displays your last hit range in blocks.",
		IDR_REACH_PNG, "") {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onAttack(AttackEvent& event);

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
