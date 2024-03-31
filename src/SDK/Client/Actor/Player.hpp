#pragma once

#include "Mob.hpp"
#include "../Container/PlayerInventory.hpp"
#include "../Block/Block.hpp"

FK(Gamemode)

class Player : public Mob {
public:
    BUILD_ACCESS(this, PlayerInventory*, playerInventory, GET_OFFSET("Player::playerInventory")) // Player::getSupplies return x 8
    BUILD_ACCESS(this, std::string, playerName, GET_OFFSET("Player::playerName"));
    BUILD_ACCESS(this, Gamemode*, gamemode, GET_OFFSET("Player::gamemode"));

    float getBreakprogress(Block block);
};