#pragma once

#include "../HUDModule.hpp"

#include "Events/Game/AttackEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"

using Duration = std::chrono::duration<double>;

struct HitInfo {
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorStableTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorHit;
	Vec3<float> lastActorTouchPos;
	Duration lastActorHitDelay{};
};

class HitPing : public HUDModule {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
	Duration hitDelay = std::chrono::milliseconds(0);
	float pingReach{};

	std::map<uint64_t, std::pair<bool, HitInfo>> actorsHit;

public:
	HitPing() : HUDModule(21, "Hit Ping", "Measures your hit delay!",
		IDR_PING_PNG, "") {

	};

	void onAttack(AttackEvent& event);

	void onPacketReceive(PacketEvent& event);

	void onTick(TickEvent& event);

	void ClearOldHits();

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customInit() override;

	void customCleanup() override;
};
