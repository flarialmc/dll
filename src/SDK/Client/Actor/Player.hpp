#pragma once

#include "Mob.hpp"
#include "../Container/PlayerInventory.hpp"

class Player : public Mob {
public:
	BUILD_ACCESS(this, PlayerInventory*, playerInventory, 0x848) // Player::getSupplies return x 8
};