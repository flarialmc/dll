#pragma once

#include "../../Client/Events/Listener.hpp"
#include "../../Client/Events/Input/KeyEvent.hpp"
#include "../../Client/Events/Render/RenderEvent.hpp"
#include "../../Client/Events/EventManager.hpp"
#include "Console.hpp"
#include "../Scripting.hpp"

class ConsoleService : public Listener {
public:
    bool enabled = false;

    void onKey(KeyEvent &event) {
        if (event.getAction() != 1) return;
        if (event.getKey() != 123) return;
        enabled = !enabled;
        if(enabled) {
            SDK::clientInstance->releaseMouse();
        }else {
            SDK::clientInstance->grabMouse();
        }
    }

    void onRender(RenderEvent &event) {
        if(!enabled) return;
        Scripting::console.render();
    }

    ConsoleService() {
        Listen(this, KeyEvent, &ConsoleService::onKey);
        Listen(this, RenderEvent, &ConsoleService::onRender);
    }

    ~ConsoleService() {
        Deafen(this, KeyEvent, &ConsoleService::onKey);
        Deafen(this, RenderEvent, &ConsoleService::onRender);
    }

};