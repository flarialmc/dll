#pragma once

#include <Client/Client.hpp>
#include <Client/Events/Input/KeyEvent.hpp>
#include <Client/Events/EventManager.hpp>

class SaveConfigListener : public Listener {
public:
    void onKey(KeyEvent &event) {
        if (event.getKey() == VK_F7 && static_cast<ActionType>(event.getAction()) == ActionType::Released) {
            Client::SaveSettings();
            Client::SavePrivate();
            FlarialGUI::Notify("Config Saved");
        }
    }

    SaveConfigListener() {
        Listen(this, KeyEvent, &SaveConfigListener::onKey);
    }

    ~SaveConfigListener() {
        Deafen(this, KeyEvent, &SaveConfigListener::onKey);
    }
};
