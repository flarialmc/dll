#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"


class OpponentReach : public Module {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
	float opponentReach{};

	std::map<int64_t, Actor*> hitActors;
public:

	OpponentReach(): Module("Opponent Reach", "Shows your opponent's last hit range!",
			IDR_REACH_PNG, "") {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onPacketReceive(PacketEvent& event);

	void onTick(TickEvent& event);
};

