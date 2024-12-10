#pragma once

#include <Utils/Utils.hpp>
#include "GLMatrix.hpp"

struct FrameTransform {
    GLMatrix modelView {};
    Vec3<float> origin {};
    Vec2<float> fov {};
};