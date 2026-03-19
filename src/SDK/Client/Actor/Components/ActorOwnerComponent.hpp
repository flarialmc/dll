#pragma once

#include "../EntityContext.hpp"

class ActorOwnerComponent : public IEntityComponent {
public:
    std::unique_ptr<class Actor> actor;
};
static_assert(sizeof(ActorOwnerComponent) == 0x8);