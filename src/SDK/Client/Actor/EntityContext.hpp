#pragma once

using EntityId = uint32_t;

namespace V1_20_50 {
    class EntityRegistry {
        char pad0x0[0x30];
    public:
        uintptr_t &registry;
    };

    class EntityContext {
    public:
        EntityRegistry& registry;
        uintptr_t &basicReg;
        EntityId id;
    };
}

namespace V1_20_40 {
    class EntityContext {
    public:
        uintptr_t &basicReg;
        EntityId id;
    };
}