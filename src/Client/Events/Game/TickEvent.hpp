
#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Actor/Actor.hpp"

class TickEvent : public Event {

    Actor* actor;

public:
    Actor* getActor() {
        return this->actor;
    }

    explicit TickEvent(Actor* e) {
        this->actor = e;
    }
};