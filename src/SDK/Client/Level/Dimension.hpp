#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Weather.hpp"

class Dimension {
public:
    BUILD_ACCESS(this, Weather*, weather, GET_OFFSET("Dimension::weather"));
    std::string getName() { return hat::member_at<std::string>(this, GET_OFFSET("Dimension::name")); }
};