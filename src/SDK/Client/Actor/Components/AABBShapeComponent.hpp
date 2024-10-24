#pragma once

#include "../../../../Utils/Utils.hpp"
#include "../EntityContext.hpp"

struct AABBShapeComponent : IEntityComponent {
    AABB aabb;
    Vec2<float> size;
};
static_assert(sizeof(AABBShapeComponent) == 0x20);
