#pragma once

#include "../../../Utils/Utils.hpp"

struct Matrix {
public:

	inline Matrix correct() const {
		Matrix ret;
		return ret;
	}

	inline bool OWorldToScreen(Vec3<float> origin, Vec3<float> pos, Vec2<float>& screen, Matrix projectionMatrix, Vec2<float> clientUIScreenSize) {
		return true;
	}
};
