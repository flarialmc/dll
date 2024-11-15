#pragma once

#include "../EntityContext.hpp"

struct OnGroundFlagComponent : IEntityComponent {};
static_assert(sizeof(OnGroundFlagComponent) == 0x1);