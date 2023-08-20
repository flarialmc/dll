#pragma once

#include "Mob.hpp"
#include "../Container/PlayerInventory.hpp"
#include "../Block/Block.hpp"

FK(Gamemode)

class Player : public Mob {
public:
	BUILD_ACCESS(this, PlayerInventory*, playerInventory, 0x7C0) // Player::getSupplies return x 8
	BUILD_ACCESS(this, std::string, playerName, 0x1C78);
	BUILD_ACCESS(this, Gamemode*, gamemode, 0xE70);

	float getBreakprogress(Block block);
};