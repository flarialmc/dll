#pragma once
#include "Input/KeyEvent.hpp"
#include "Input/MouseEvent.hpp"
#include "Render/RenderEvent.hpp"
#include "Game/TickEvent.hpp"

class Listener {
public:
    std::string name;

    virtual void onKey(KeyEvent &event) {};
    virtual void onMouse(MouseEvent &event) {};
    virtual void onRender(RenderEvent &event) {};
    virtual void onLocalTick(TickEvent &event) {};
    virtual void onTick(TickEvent &event) {};
};
