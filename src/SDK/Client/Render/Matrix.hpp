#pragma once

#include <glm/mat4x4.hpp>
#include "../../../Utils/Utils.hpp"
#include "GLMatrix.hpp"

// By Intop
struct Matrix {
public:
	glm::mat4x4 matrix;

	[[nodiscard]] inline Matrix correct() const {
		Matrix ret{};
		ret.matrix = glm::transpose(this->matrix);
		return ret;
	}

	inline bool OWorldToScreen(Vec3<float> origin, Vec3<float> pos, Vec2<float>& screen, Matrix projectionMatrix, Vec2<float> clientUIScreenSize) const {
		pos = pos.sub(origin);

		glm::vec4 transformedPos = this->matrix * glm::vec4(pos.x, pos.y, pos.z, 1.0f);

		if (transformedPos.z > 0)
			return false;

		float mX = clientUIScreenSize.x / 2.0f;
		float mY = clientUIScreenSize.y / 2.0f;

		screen.x = mX + (mX * transformedPos.x / -transformedPos.z * projectionMatrix.matrix[0][0]);
		screen.y = mY - (mY * transformedPos.y / -transformedPos.z * projectionMatrix.matrix[1][1]);
		
		return true;
	}

    bool WorldToScreen(Vec3<float> pos, Vec2<float> &screen);

    GLMatrix *getMatrixCorrection();

    float transformz(const Vec3<float> &p);

    float transformy(const Vec3<float> &p);

    float transformx(const Vec3<float> &p);
};
