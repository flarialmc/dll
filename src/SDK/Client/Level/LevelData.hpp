#pragma once

#include <string>
#include <unordered_map>

#include "libhat/Access.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class LevelData {
public:
    std::string getLevelName() { return hat::member_at<std::string>(this, GET_OFFSET("LevelData::worldName")); }

};
