#pragma once

using EntityId = uint32_t;

class EntityRegistry {
	char pad0x0[0x30];
public:
	uintptr_t& registry;
};

class EntityContext {
public:
	EntityRegistry& registry;
	uintptr_t& basicReg;
	EntityId id;
};