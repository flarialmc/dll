#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class RenderCurrentFrameEvent : public Event, public Cancellable {};