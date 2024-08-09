#pragma once

#include <utility>

#include "../../../SDK/Client/Render/ResourceLocation.hpp"
#include "../Cancellable.hpp"
#include "../Event.hpp"

class GetTextureEvent : public Event, Cancellable {
private:
    ResourceLocation* location;
public:
    explicit GetTextureEvent(ResourceLocation* resourceLocation) {
        location = resourceLocation;
    }
};