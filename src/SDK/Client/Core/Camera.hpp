#pragma once

#include "../Render/Matrix.hpp"
#include "../../../Utils/Utils.hpp"

class Camera {
public:
    BUILD_ACCESS(this, Matrix, viewMatrix, GET_OFFSET("Camera::viewMatrix"));
    BUILD_ACCESS(this, Matrix, projectionMatrix, GET_OFFSET("Camera::projectionMatrix"));
};
