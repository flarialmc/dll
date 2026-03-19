#pragma once

#include "../HUDModule.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"

class OpponentReach : public HUDModule {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
	float opponentReach{};

	std::map<int64_t, Actor*> hitActors;

public:
	OpponentReach(): HUDModule(22, "Opponent Reach", "Shows your opponent's last hit range!",
			IDR_REACH_PNG, "") {

	}

	void onPacketReceive(PacketEvent& event);

	void onTick(TickEvent& event);

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customInit() override;

	void customCleanup() override;
};
