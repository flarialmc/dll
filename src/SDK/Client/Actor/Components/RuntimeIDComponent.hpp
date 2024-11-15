#pragma once
#include <cstdint>

struct RuntimeIDComponent : IEntityComponent {
    int64_t runtimeID;
};
static_assert(sizeof(RuntimeIDComponent) == 0x8);
