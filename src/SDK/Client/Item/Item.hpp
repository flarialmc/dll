#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "../../src/Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class Item {
public:
    BUILD_ACCESS(this, std::string, AtlasTextureFile, GET_OFFSET("Item::AtlasTextureFile"));
    BUILD_ACCESS(this, std::string, Namespace, GET_OFFSET("Item::Namespace"));
    BUILD_ACCESS(this, std::string, name, GET_OFFSET("Item::name"));
};