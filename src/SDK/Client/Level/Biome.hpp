#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Weather.hpp"

class Biome {
public:
    BUILD_ACCESS(this, float, temparature, GET_OFFSET("Biome::temparature"));
};