#pragma once

#include "Client/Core/ClientInstance.hpp"
#include "Client/Render/ScreenView/ScreenView.hpp"
#include "Client/Network/Raknet/RaknetConnector.hpp"

namespace SDK {
	extern ClientInstance* clientInstance;
    extern ScreenView* screenView;
    extern bool hasInstanced;
    extern std::string CurrentScreen;
    extern RaknetConnector* raknetConnector;
}