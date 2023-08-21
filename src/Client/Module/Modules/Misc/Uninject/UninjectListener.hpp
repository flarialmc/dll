#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"
class UninjectListener : public Listener {

public:

    Module* module;

    void onKey(KeyEvent& event) override {
      
            if (event.GetKey() == VK_F8 && static_cast<ActionType>(event.GetAction()) == ActionType::RELEASED) {
                ModuleManager::terminate();
                Client::disable = true;

                //if (SDK::clientInstance->getLocalPlayer() == nullptr) {
                //    ModuleManager::terminate();
                //    Client::disable = true;
                //}
                //else {
                //
                //     FlarialGUI::Notify("Cannot eject in a world");
                //
                //}
            }
   
       
    }

public:
    explicit UninjectListener(const char string[5]) {
        this->name = string;
    }
};
