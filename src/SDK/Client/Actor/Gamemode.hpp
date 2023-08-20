#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Player.hpp"

class Gamemode {
public:
	BUILD_ACCESS(this, Player*, player, 0x8);
	BUILD_ACCESS(this, float, lastBreakProgress, 0x20);
};