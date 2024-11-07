#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class RenderChunkCoordinatorPreRenderTickEvent : public Event, public Cancellable {};