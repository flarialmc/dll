#pragma once

#include <Utils/Memory/Memory.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

struct ViewRenderData {
    BUILD_ACCESS(this, Vec3<float>, cameraPos, GET_OFFSET("ViewRenderData::cameraPos"));
    BUILD_ACCESS(this, Vec3<float>, cameraTargetPos, GET_OFFSET("ViewRenderData::cameraTargetPos"));
};
