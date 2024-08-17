#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Player.hpp"

class Gamemode {
public:
    Player* getPlayer() {
        return hat::member_at<Player*>(this, GET_OFFSET("Gamemode::player"));
    };
    float getLastBreakProgress() {
        return hat::member_at<float>(this, GET_OFFSET("Gamemode::lastBreakProgress"));
    };
};