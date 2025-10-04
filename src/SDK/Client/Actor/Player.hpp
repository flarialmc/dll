#pragma once

#include "Actor.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

// Forward declarations
class PlayerInventory;
class Gamemode;

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