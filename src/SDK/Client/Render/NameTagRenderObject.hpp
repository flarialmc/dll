#pragma once

#include <Utils/Memory/Memory.hpp>

struct NameTagRenderObject {
    BUILD_ACCESS(this, std::string, nameTag, GET_OFFSET("NameTagRenderObject::nameTag"));
    BUILD_ACCESS(this, Vec3<float>, pos, GET_OFFSET("NameTagRenderObject::pos"));
};
