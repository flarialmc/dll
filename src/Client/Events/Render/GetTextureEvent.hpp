#pragma once

#include <utility>

#include "../../../SDK/Client/Render/ResourceLocation.hpp"
#include "../Cancellable.hpp"
#include "../Event.hpp"

class GetTextureEvent : public Event, Cancellable {
public:
    ResourceLocation* location;

    explicit GetTextureEvent(ResourceLocation* resourceLocation) {
        location = resourceLocation;
    }
};