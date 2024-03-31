#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class Weather {
public:
    BUILD_ACCESS(this, float, lightingLevel, GET_OFFSET("Weather::lightingLevel"));
    BUILD_ACCESS(this, float, rainLevel, GET_OFFSET("Weather::rainLevel"));

};