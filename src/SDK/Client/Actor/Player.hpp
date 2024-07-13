#pragma once

#include "../Container/PlayerInventory.hpp"
#include "Actor.hpp"

FK(Gamemode)

class Player : public Actor {
public:
    BUILD_ACCESS(this, PlayerInventory*, playerInventory, GET_OFFSET("Player::playerInventory")) // Player::getSupplies return x 8
    BUILD_ACCESS(this, std::string, playerName, GET_OFFSET("Player::playerName"));
    BUILD_ACCESS(this, Gamemode*, gamemode, GET_OFFSET("Player::gamemode"));
};