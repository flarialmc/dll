#pragma once

#include "../EntityContext.hpp"

class ActorOwnerComponent : public IEntityComponent {
public:
    class Actor* actor;
};
static_assert(sizeof(ActorOwnerComponent) == 0x8);