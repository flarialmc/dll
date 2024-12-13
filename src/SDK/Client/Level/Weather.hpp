#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>

class Weather {
public:
    BUILD_ACCESS(this, float, lightingLevel, GET_OFFSET("Weather::lightingLevel"));
    BUILD_ACCESS(this, float, rainLevel, GET_OFFSET("Weather::rainLevel"));
};