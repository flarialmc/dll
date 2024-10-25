#pragma once

#include "../../../../Utils/Utils.hpp"
#include "../EntityContext.hpp"

struct RenderPositionComponent {
    Vec3<float> renderPos;
};
static_assert(sizeof(RenderPositionComponent) == 0xC);