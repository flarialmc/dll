#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#pragma once

class Minecraft {
public:
    BUILD_ACCESS(this, float*, timer, GET_OFFSET("Minecraft::timer"));
};