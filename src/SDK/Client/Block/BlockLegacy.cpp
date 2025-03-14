#include "BlockLegacy.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

std::string BlockLegacy::getName() {
    return hat::member_at<std::string>(this, GET_OFFSET("BlockLegacy::name"));
}

std::string BlockLegacy::getNamespace() {
    return hat::member_at<std::string>(this, GET_OFFSET("BlockLegacy::namespace"));
}
