#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class MinecraftUIRenderContext;

class SetupAndRenderEvent : public Event {
private:
    MinecraftUIRenderContext *muirc;

public:
    explicit SetupAndRenderEvent(MinecraftUIRenderContext *muirc) {
        this->muirc = muirc;
    }

    [[nodiscard]] MinecraftUIRenderContext *getMuirc() const {
        return muirc;
    }
};