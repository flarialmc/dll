#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include <glm/glm/glm.hpp>

class GlmRotateEvent : public Event, public Cancellable {
private:
    glm::mat4x4& matrix;
    float angle;
    glm::vec3 axis;

public:
    GlmRotateEvent(glm::mat4x4& mat, float ang, const glm::vec3& ax)
        : matrix(mat), angle(ang), axis(ax) {}

    glm::mat4x4& getMatrix() { return matrix; }
    float getAngle() const { return angle; }
    const glm::vec3& getAxis() const { return axis; }
};