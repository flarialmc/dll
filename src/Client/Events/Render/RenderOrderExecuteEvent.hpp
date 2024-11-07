#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class RenderOrderExecuteEvent : public Event, public Cancellable {};