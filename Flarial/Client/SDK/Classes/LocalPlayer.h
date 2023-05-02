#pragma once
#include "Actor.h"

class LocalPlayer {
	char pad_0000[0x2478];
public:
	std::string username;
public:
	auto getSupplies(void) -> PlayerInventory* {
		return *(PlayerInventory**)((uintptr_t)(this) + 0xDB8);
	};
public:
	auto getGameMode(void) -> GameMode* {
		return *(GameMode**)((uintptr_t)(this) + 0x1658);
	};

	void setSprinting(bool status) {
		return Utils::CallVFunc<284, void, bool>(this, status);
	}
};