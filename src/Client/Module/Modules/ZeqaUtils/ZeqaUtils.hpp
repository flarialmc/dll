#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ModalFormRequestPacket.hpp"
#include <lib/json/json.hpp>

#include "Events/Network/PacketEvent.hpp"

class ZeqaUtils : public Module {

public:

	ZeqaUtils(): Module("Zeqa Utils", "Handy utilities for Zeqa",
			IDR_ZEQA_PNG, "", false, {"auto requeue", "auto rq"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;


	void onPacketReceive(PacketEvent& event);
};

