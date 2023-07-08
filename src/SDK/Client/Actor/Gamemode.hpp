#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Player.hpp"


class Gamemode {
public:
	BUILD_ACCESS(this, Player*, player, 0x8);
};