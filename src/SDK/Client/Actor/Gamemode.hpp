#pragma once

#include <Utils/Memory/Memory.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

FK(Player)

class Gamemode {
public:
    Player* getPlayer() {
        return hat::member_at<Player*>(this, GET_OFFSET("Gamemode::player"));
    };
    float getLastBreakProgress() {
        return hat::member_at<float>(this, GET_OFFSET("Gamemode::lastBreakProgress"));
    };
};