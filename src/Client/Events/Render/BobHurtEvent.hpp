#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "glm/glm/fwd.hpp"

class BobHurtEvent : public Event {
public:
    glm::mat4* matrix;
    explicit BobHurtEvent(glm::mat4* matrix) : matrix(matrix) {}
};

