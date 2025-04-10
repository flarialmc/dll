#pragma once 
#include "../Event.hpp"
#include "../../../Utils/Utils.hpp"

class ActorRenderDispatcherEvent : public Event {
    Actor* ent;
    Vec2<float> rot;
    Vec3<float> pos;
public:
    explicit ActorRenderDispatcherEvent(Actor* acotr, Vec2<float> rot, Vec3<float> pos) {
        this->ent = acotr;
        this->rot = rot;
        this->pos = pos;
    }

    Actor* getEntity() {
        return ent;
    }

    Vec2<float> getRot() {
        return rot;
    }

    Vec3<float> getPos() {
        return pos;
    }
};