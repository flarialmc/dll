#pragma once

#include "../../src/Utils/Memory/Memory.hpp"

class Item {
public:
	BUILD_ACCESS(this, std::string, AtlasTextureFile, 0x8); 
	BUILD_ACCESS(this, std::string, Namespace, 0x118); 
	BUILD_ACCESS(this, std::string, name, 0x0C0); 
};