#pragma once
#include "glm/glm/detail/type_quat.hpp"

class SomeCameraRelatedEvent : public Event
{
public:
    glm::quat orientation;
    glm::vec3 position;
    float aspectRatio = 0;
    float fieldOfView = 0;
    float nearPlane = 0;
    float farPlane = 0;
    glm::mat4 postViewTransform {};
    glm::mat4 savedProjection {};
    glm::mat4 savedModelView {};

    explicit SomeCameraRelatedEvent(
        const glm::quat& orientation,
        const glm::vec3& position,
        float aspectRatio,
        float fieldOfView,
        float nearPlane,
        float farPlane,
        const glm::mat4& postViewTransform,
        const glm::mat4& savedProjection,
        const glm::mat4& savedModelView
    ) : orientation(orientation),
        position(position),
        aspectRatio(aspectRatio),
        fieldOfView(fieldOfView),
        nearPlane(nearPlane),
        farPlane(farPlane),
        postViewTransform(postViewTransform),
        savedProjection(savedProjection),
        savedModelView(savedModelView) {}

    friend class Camera;
    friend class CameraController;

    [[nodiscard]] glm::quat getOrientation() const { return this->orientation; }
    [[nodiscard]] glm::vec3 getPosition() const { return this->position; }
    [[nodiscard]] float getAspectRatio() const { return this->aspectRatio; }
    [[nodiscard]] float getFieldOfView() const { return this->fieldOfView; }
    [[nodiscard]] float getNearPlane() const { return this->nearPlane; }
    [[nodiscard]] float getFarPlane() const { return this->farPlane; }
    [[nodiscard]] glm::mat4 getPostViewTransform() const { return this->postViewTransform; }
    [[nodiscard]] glm::mat4 getSavedProjection() const { return this->savedProjection; }
    [[nodiscard]] glm::mat4 getSavedModelView() const { return this->savedModelView; }
};
