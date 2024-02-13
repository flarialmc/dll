#pragma once

#include "../Render/Matrix.hpp"
#include "../../../Utils/Utils.hpp"

class Camera {
public:
	BUILD_ACCESS(this, Matrix, viewMatrix, 0xC0);
	BUILD_ACCESS(this, Matrix, projectionMatrix, 0x100);
};
