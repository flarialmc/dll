#pragma once

#include "SDK/Client/Actor/EntityContext.hpp"

struct MobBodyRotationComponent : IEntityComponent {
    float mYBodyRot;
    float mYBodyRotO;
};
static_assert(sizeof(MobBodyRotationComponent) == 0x8);
