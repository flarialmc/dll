#pragma once

#include "MatrixStack.hpp"
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

struct FrustumEdges
{
    glm::tvec3<float> topLeft;
    glm::tvec3<float> topRight;
    glm::tvec3<float> bottomLeft;
    glm::tvec3<float> bottomRight;
};

struct Frustum
{
    glm::tvec4<float> planes[6];
    FrustumEdges nearClippingPlaneEdges;
    FrustumEdges farClippingPlaneEdges;
};

namespace mce {
    class Camera {
    public:
        // MatrixStack& getWorldMatrixStack() {
        //     static auto off = GET_OFFSET("mce::Camera::worldMatrixStack");
        //     return hat::member_at<MatrixStack>(this, off);
        // }

        MatrixStack viewMatrixStack;
        MatrixStack worldMatrixStack;
        MatrixStack projectionMatrixStack;

        glm::mat4 mInverseViewMatrix;
        glm::vec3 mRight;
        glm::vec3 mUp;
        glm::vec3 mForward;
        glm::vec3 mPosition;
        float mAspectRatio;
        float mFov;
        float mZNear;
        float mZFar;
        Frustum mFrustum;

        MatrixStack& getProjectionMatrixStack() { return projectionMatrixStack; }
        MatrixStack& getViewMatrixStack() { return viewMatrixStack; }
        MatrixStack& getWorldMatrixStack() { return worldMatrixStack; }
    };
}
