#pragma once

#include "../Event.hpp"

enum class Perspective {
    FirstPerson = 0,
    ThirdPersonBack = 1,
    ThirdPersonFront = 2
};

class PerspectiveEvent : public Event {
private:
    Perspective perspective;
public:
    Perspective getPerspective() {
        return this->perspective;
    }

    void setPerspective(Perspective e) {
        this->perspective = e;
    }

    explicit PerspectiveEvent(Perspective e) {
        this->perspective = e;
    }
};
