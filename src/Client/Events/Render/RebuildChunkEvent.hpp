#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class RebuildChunkEvent : public Event, public Cancellable {};