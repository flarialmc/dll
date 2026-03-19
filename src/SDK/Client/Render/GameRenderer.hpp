#pragma once

#include "GLMatrix.hpp"
#include <glm/glm/mat4x4.hpp>
#include <libhat/Access.hpp>

class GameRenderer {
public:
    glm::mat4x4& getLastViewMatrix() {
        // 1st matrix slot in GameRenderer (0x358), per-frame camera view matrix
        return hat::member_at<glm::mat4x4>(this, 0x358);
    }

    glm::mat4x4& getLastProjectionMatrix() {
        // 3rd matrix slot in GameRenderer (0x3D8), contains perspective projection
        return hat::member_at<glm::mat4x4>(this, 0x3D8);
    }
};
