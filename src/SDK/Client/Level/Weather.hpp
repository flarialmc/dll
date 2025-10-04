#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class Weather {
public:
    BUILD_ACCESS(this, float, lightningLevel, GET_OFFSET("Weather::lightningLevel"));
    BUILD_ACCESS(this, float, rainLevel, GET_OFFSET("Weather::rainLevel"));
};