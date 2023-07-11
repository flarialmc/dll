#pragma once

#include "Mob.hpp"
#include "../Container/PlayerInventory.hpp"

FK(Gamemode)

class Player : public Mob {
public:
	BUILD_ACCESS(this, PlayerInventory*, playerInventory, 0x848) // Player::getSupplies return x 8
	BUILD_ACCESS(this, std::string, playerName, 0x1D08);
	BUILD_ACCESS(this, Gamemode*, gamemode, 0xEF8);
};