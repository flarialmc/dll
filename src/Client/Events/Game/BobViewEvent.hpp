#pragma once

#include "../Event.hpp"
#include <glm/glm/glm.hpp>

class BobViewEvent : public Event {
public:
    glm::mat4* matrix;
};
