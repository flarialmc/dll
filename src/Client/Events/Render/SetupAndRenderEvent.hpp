#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"

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