#pragma once

#include "../../../../Utils/Memory/Memory.hpp"

class UIComponent {
public:
	
};

class MinecraftCustomUIRenderer {	
public:
	BUILD_ACCESS(this, float, state, 0x10);
};

class CustomRenderComponent : public UIComponent {
public:
	BUILD_ACCESS(this, MinecraftCustomUIRenderer*, renderer, 0x18);
};
