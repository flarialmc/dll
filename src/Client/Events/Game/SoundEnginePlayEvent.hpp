#pragma once

#include "../../../Utils/Utils.hpp"
#include "Events/Event.hpp"

class SoundEnginePlayEvent : public Event {
    public:
        const std::string& name;
        std::string pos;
        float a4;
        float a5;

        SoundEnginePlayEvent(const std::string& name, Vec3<float> pos, float a4, float a5)
            : name(name), pos(pos), a4(a4), a5(a5) {
        }
};
