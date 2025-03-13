#pragma once

#include <Scripting/Scripting.hpp>
#include "../../../../Client.hpp"

class SaveConfigListener : public Listener {
public:
    void onKey(KeyEvent &event) {
        if (event.getKey() == VK_F7 && static_cast<ActionType>(event.getAction()) == ActionType::Released) {
            ModuleManager::restart();
        }
    }

    SaveConfigListener() {
        Listen(this, KeyEvent, &SaveConfigListener::onKey);
    }

    ~SaveConfigListener() {
        Deafen(this, KeyEvent, &SaveConfigListener::onKey);
    }
};
