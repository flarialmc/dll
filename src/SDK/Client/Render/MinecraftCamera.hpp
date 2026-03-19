//
// Created by Leslie on January 11, 2026.
//

#pragma once
#include "glm/glm/detail/type_quat.hpp"

namespace MinecraftCamera {
    struct CameraComponent : IEntityComponent {
        HashedString mId;
        glm::quat mOrientation {};
        glm::vec3 mPosition {};
        float mAspectRatio {};
        float mFieldOfView {};
        float mNearPlane {};
        float mFarPlane {};
        glm::mat4 mPostViewTransform {};
        glm::mat4 mSavedProjection {};
        glm::mat4 mSavedModelView {};
    };
}