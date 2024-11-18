#pragma once

#include "Client/Core/ClientInstance.hpp"
#include "Client/Render/ScreenView/ScreenView.hpp"
#include "Client/Network/Packet/Packet.hpp"
#include <cctype>

namespace SDK {
    extern ClientInstance *clientInstance;
    extern ScreenView *screenView;
    extern bool hasInstanced;
    extern std::string currentScreen;
    extern uint64_t serverPing;
    bool containsIgnoreCase(const std::string& mainString, const std::string& searchString);
    extern int getServerPing();

    extern void setCurrentScreen(const std::string& layer);

    extern std::string getCurrentScreen();

    extern std::string getServerIP();

    extern std::shared_ptr<Packet> createPacket(int id);
    extern bool isHovered(Vec4<float> box, Vec2<float> mouse);
}