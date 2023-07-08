#pragma once
#include "SDK.hpp"

RaknetConnector* SDK::raknetConnector = nullptr;
ClientInstance* SDK::clientInstance = nullptr;
ScreenView* SDK::screenView = nullptr;
std::string SDK::CurrentScreen;
bool SDK::hasInstanced = false;