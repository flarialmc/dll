#pragma once

#include <glm/glm/mat4x4.hpp>
#include <Utils/Utils.hpp>
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

    static bool WorldToScreen(Vec3<float> pos, Vec2<float> &screen);

    static glm::mat4 getMatrixCorrection(GLMatrix mat);
};
