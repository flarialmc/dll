#pragma once

#include "../Module.hpp"
#include "../../../Hook/Hooks/Game/PacketHooks.hpp"


class InstantHurtAnimation : public Module {
private:
	std::unordered_map<int64_t, std::chrono::steady_clock::time_point> hitEntities;
public:
	InstantHurtAnimation() : Module("Insta Hurt Animation", "Hurt animation becomes ping independent, helps time hits.",
		IDR_COMBO_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onPacketReceive(PacketEvent& event);

	void onAttack(AttackEvent& event);

	void ClearOldHits();
};
