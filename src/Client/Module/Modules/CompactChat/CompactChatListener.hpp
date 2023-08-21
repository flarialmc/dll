#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <variant>
#include <utility>
#include <Windows.h>

class CompactChatListener : public Listener {

public:
	Module* module;
	static inline std::vector<std::pair<std::string, int>> dups = {};
	static inline std::string prev;
	bool prevPacketSent = false;

	void onPacketReceive(PacketEvent& event) {
		if (module->settings.getSettingByName<bool>("enabled")->value) {
			if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
				std::string msg = reinterpret_cast<TextPacket*>(event.getPacket().get())->message;

				if (!prevPacketSent) {
					prevPacketSent = true;
					return;
				}

				if (msg == prev) {
					auto last = dups.back();
					if (last.first == msg) last.second++;
					else dups.push_back(std::make_pair(msg, 1));

					//std::cout << "Duplicate message detected yes" << std::endl;
				}

				if (dups.size() > 100) dups.erase(dups.begin());

				prevPacketSent = false;

				prev = msg;
			}
		}
	}

public:
	explicit CompactChatListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;

		dups.resize(420);
	}
};

