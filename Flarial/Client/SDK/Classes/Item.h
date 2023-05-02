#pragma once
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"

class Item {
public:
	auto getNameId(void) -> std::string {
		return *(std::string*)((uintptr_t)(this) + 0x110);
	};
public:
	BUILD_ACCESS(this, uint16_t, maxStackSize, 0x90);
	BUILD_ACCESS(this, std::string, name, 0xC0);
};