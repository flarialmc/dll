#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/GUI/RectangleArea.hpp"
#include <format>

class MinecraftUIRenderContext;

class HudCursorRendererRenderEvent : public Event, public Cancellable {
private:
    RectangleArea* area;
    MinecraftUIRenderContext* muirc;
public:
    HudCursorRendererRenderEvent(RectangleArea* area, MinecraftUIRenderContext* muirc) : area(area) , muirc(muirc) {}

    MinecraftUIRenderContext* getMinecraftUIRenderContext() {
        return muirc;
    }

    RectangleArea* getArea() {
        return area;
    }

    void setArea(RectangleArea newArea) {
        *this->area = newArea;
    }
};