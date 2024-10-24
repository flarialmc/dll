#pragma once

#include "MatrixStack.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

namespace mce {
    class Camera {
    public:
        MatrixStack& getWorldMatrixStack() {
            static auto off = GET_OFFSET("mce::Camera::worldMatrixStack");
            return hat::member_at<MatrixStack>(this, off);
        }
    };
}
