#pragma once

#include "../Module.hpp"

#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "SDK/Client/Network/Packet/LoginPacket.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


class PlayerNotifier : public Module {
	int totalPlayers = 0;
public:
	PlayerNotifier() : Module("Player Notifier", "Notifies you when a player is in the server.",
		IDR_NOTIFICATION_PNG, "P", false, {"alert", "notification"}) {
		/*Module::setup();
		defaultConfig();
		loadSettings();*/
	};

	void defaultConfig() override;
	void onEnable() override;

	void onDisable() override;

	void onSetup() override;

	void loadSettings(bool softLoad) override;

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