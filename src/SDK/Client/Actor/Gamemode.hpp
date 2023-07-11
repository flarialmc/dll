#pragma once

#include "../../src/Utils/Memory/Memory.hpp"

FK(Player)

class Gamemode {
public:
	BUILD_ACCESS(this, Player*, player, 0x8);
};