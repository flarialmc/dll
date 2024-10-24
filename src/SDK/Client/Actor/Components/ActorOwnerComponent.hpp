#pragma once

#include "../EntityContext.hpp"

class ActorOwnerComponent : IEntityComponent {
public:
    class Actor* actor;
};
static_assert(sizeof(ActorOwnerComponent) == 0x8);