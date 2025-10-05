#pragma once

#include "../Module.hpp"

#include "Events/Game/AttackEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"

using Duration = std::chrono::duration<double>;

struct HitInfo {
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorStableTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorHit;
	Vec3<float> lastActorTouchPos;
	Duration lastActorHitDelay{};
};

class HitPing : public Module {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
	Duration hitDelay = std::chrono::milliseconds(0);
	float pingReach{};

	std::map<uint64_t, std::pair<bool, HitInfo>> actorsHit;
public:
	HitPing() : Module("Hit Ping", "Measures your hit delay!",
		IDR_PING_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onAttack(AttackEvent& event);

	void onPacketReceive(PacketEvent& event);

	void onTick(TickEvent& event);

	void ClearOldHits();
};

