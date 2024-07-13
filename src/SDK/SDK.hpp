#pragma once

#include "Client/Core/ClientInstance.hpp"
#include "Client/Render/ScreenView/ScreenView.hpp"
#include "Client/Network/Packet/Packet.hpp"

namespace SDK {
    extern ClientInstance *clientInstance;
    extern ScreenView *screenView;
    extern bool hasInstanced;
    extern std::string currentScreen;
    extern std::chrono::steady_clock::time_point lastSetCurrentScreenTime;

    extern int getServerPing();

    extern void setCurrentScreen(const std::string& layer);

    extern std::string getServerIP();

    extern std::shared_ptr<Packet> createPacket(int id);


}