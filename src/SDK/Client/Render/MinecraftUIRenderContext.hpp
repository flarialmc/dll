#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class MinecraftUIRenderContext {
public:
	BUILD_ACCESS(this, ClientInstance*, clientInstance, 0x8);
};