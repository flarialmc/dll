#pragma once
#include <cstdint>

struct ActorUniqueIDComponent : IEntityComponent {
    int64_t mActorUniqueID;
};
static_assert(sizeof(ActorUniqueIDComponent) == 0x8);
