#pragma once
#include "SDK.hpp"

ClientInstance* SDK::clientInstance = nullptr;
ScreenView* SDK::screenView = nullptr;
std::string SDK::CurrentScreen;
bool SDK::hasInstanced = false;