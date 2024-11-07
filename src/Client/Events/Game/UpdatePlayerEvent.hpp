#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Actor/Actor.hpp"

class UpdatePlayerEvent : public Event, public Cancellable {};