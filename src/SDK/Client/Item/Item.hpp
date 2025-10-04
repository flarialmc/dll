#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class Item {
public:
    BUILD_ACCESS(this, std::string, AtlasTextureFile, GET_OFFSET("Item::AtlasTextureFile"));
    BUILD_ACCESS(this, std::string, Namespace, GET_OFFSET("Item::Namespace"));
    BUILD_ACCESS(this, std::string, name, GET_OFFSET("Item::name"));
};