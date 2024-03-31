#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Player.hpp"

class Gamemode {
public:
    BUILD_ACCESS(this, Player*, player, GET_OFFSET("Gamemode::player"));
    BUILD_ACCESS(this, float, lastBreakProgress, GET_OFFSET("Gamemode::lastBreakProgress"));
};