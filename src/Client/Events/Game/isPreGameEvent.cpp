#include "isPreGameEvent.hpp"

isPreGameEvent::isPreGameEvent(bool currentState) {
    this->state = currentState;
}

void isPreGameEvent::setState(bool newState) {
    this->state = newState;
}

bool isPreGameEvent::getState() {
    return this->state;
}
