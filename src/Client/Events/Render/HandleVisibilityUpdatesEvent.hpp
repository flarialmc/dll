#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class HandleVisibilityUpdatesEvent : public Event, public Cancellable {};