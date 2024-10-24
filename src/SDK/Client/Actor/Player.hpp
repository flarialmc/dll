#pragma once

#include "../Container/PlayerInventory.hpp"
#include "Actor.hpp"
#include "Gamemode.hpp"

class Player : public Actor {
public:
    PlayerInventory* getSupplies() {
        return hat::member_at<PlayerInventory*>(this, GET_OFFSET("Player::playerInventory"));
    }

    std::string& getPlayerName() {
        return hat::member_at<std::string>(this, GET_OFFSET("Player::playerName"));
    }

    Gamemode* getGamemode() {
        return hat::member_at<Gamemode*>(this, GET_OFFSET("Player::gamemode"));
    }
};