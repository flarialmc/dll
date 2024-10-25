#pragma once

#include <bitset>
#include "../EntityContext.hpp"

struct ActorDataFlagComponent {
    std::bitset<0x77> flags;
};
static_assert(sizeof(ActorDataFlagComponent) == 0x10);