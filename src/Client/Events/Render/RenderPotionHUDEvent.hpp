#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include <format>

class RenderPotionHUDEvent : public Event, public Cancellable {};