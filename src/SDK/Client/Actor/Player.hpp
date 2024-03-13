#pragma once

#include "Mob.hpp"
#include "../Container/PlayerInventory.hpp"
#include "../Block/Block.hpp"

FK(Gamemode)

class Player : public Mob {
public:
	BUILD_ACCESS(this, PlayerInventory*, playerInventory, 0x7B0) // Player::getSupplies return x 8
	BUILD_ACCESS(this, std::string, playerName, 0x1D70);
	BUILD_ACCESS(this, Gamemode*, gamemode, 0xED8);

	float getBreakprogress(Block block);

    bool canSee(const Player &actor);
};