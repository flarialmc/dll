#pragma once
#include <glm/mat4x4.hpp>

struct GLMatrix {
public:
    float matrix[16];
    float matrix_nest[4][4];
};