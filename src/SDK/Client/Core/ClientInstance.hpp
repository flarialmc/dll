#pragma once

#include "../Actor/LocalPlayer.hpp"
#include <cstdint>

class ClientInstance {
public:
	uintptr_t** VTable;

	LocalPlayer* getLocalPlayer() {
		//here we call on getLocalPlayer through the virtual function table by getting its addr
		//then casting it into a function signature LocalPlayer*(__fastcall*)(ClientInstance*))
		auto getLocalPlayerFunc = reinterpret_cast<LocalPlayer*(__fastcall*)(ClientInstance*)> (this->VTable[27]);
		return getLocalPlayerFunc(this);
	}
};