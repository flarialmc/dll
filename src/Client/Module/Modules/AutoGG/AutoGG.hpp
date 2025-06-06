#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "Events/Network/PacketEvent.hpp"


class AutoGG : public Module {

public:
	AutoGG() : Module("Auto GG", "Automatically sends a message when you win a game. Workes on\nThe Hive, Zeqa, CubeCraft, Lifeboat, Galaxite and Mineville. ",
		IDR_LIKE_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onPacketReceive(PacketEvent& event);

	void SendGG();
};

