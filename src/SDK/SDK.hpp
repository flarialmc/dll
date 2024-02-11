#pragma once

#include "Client/Core/ClientInstance.hpp"
#include "Client/Render/ScreenView/ScreenView.hpp"
#include "Client/Network/Packet/Packet.hpp"

namespace SDK {
	extern ClientInstance* clientInstance;
    extern ScreenView* screenView;
    extern bool hasInstanced;
    extern std::string CurrentScreen;
    extern std::shared_ptr<Packet> createPacket(int id);
    extern void setCI();
}