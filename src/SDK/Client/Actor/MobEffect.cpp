#pragma once

#include "MobEffect.h"
#include "../../../Utils/Memory/Memory.hpp"

std::unique_ptr<MobEffect, std::default_delete<MobEffect>>* MobEffect::getEffects() {
	static auto ref = Memory::findSig("48 8D 0D ?? ?? ?? ?? 48 8B 0C C1 48 8B 01 44 8B 4E 14 44 8B 46 04 48 8B D3 48 8B 40 18");
	int off = *reinterpret_cast<int*>(ref + 3);
	return reinterpret_cast<std::unique_ptr<MobEffect, std::default_delete<MobEffect>>*>(ref + off + 7); 
}