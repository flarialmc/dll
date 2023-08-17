#pragma once

struct EntityId {
	int id;
};

class EntityContext {
	uintptr_t* registry;
	EntityId id;
};