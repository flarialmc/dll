#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"

class RenderOutlineSelectionEvent : public Event, public Cancellable {
    Vec3<float> pos;
    ScreenContext* scn;
public:
    explicit RenderOutlineSelectionEvent(Vec3<float> pos, ScreenContext* scn) {
        this->pos = pos;
        this->scn = scn;
    }
    Vec3<float> getPos() { return pos; }
    ScreenContext* getScreenContext() { return scn; }
    AABB getBox() { return AABB(pos, pos.add(1, 1, 1)); }
};