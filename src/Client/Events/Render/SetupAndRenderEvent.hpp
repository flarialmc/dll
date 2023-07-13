
#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"

class SetupAndRenderEvent : public Event {

public:
    MinecraftUIRenderContext* muirc;
};