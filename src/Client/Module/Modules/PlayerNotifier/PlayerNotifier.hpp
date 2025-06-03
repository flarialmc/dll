#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "SDK/Client/Network/Packet/LoginPacket.hpp"


class PlayerNotifier : public Module {
	int totalPlayers = 0;
public:
	PlayerNotifier();;

	void defaultConfig() override;

	void onEnable() override;

	void onDisable() override;

	void onSetup() override;

	void loadSettings() override;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastRun = std::chrono::steady_clock::now();
	bool first = true;

	void check();

	void onTick(TickEvent& event);

	/*
	void onPacketReceive(PacketEvent &event) {
		MinecraftPacketIds id = event.getPacket()->getId();
		MinecraftPacketIds::PlayerList
		if (id == MinecraftPacketIds::Login) {
			auto pkt = reinterpret_cast<LoginPacket*>(event.getPacket());
			pkt->
		}
	}
	*/

	void settingsRender(float settingsOffset) override;

	void onKey(KeyEvent& event);;
};
