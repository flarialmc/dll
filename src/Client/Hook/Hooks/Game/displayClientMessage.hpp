#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class displayClientMessageHook : public Hook {
private:
    static void displayClientMessageDetour(GuiData* guidata, const std::string& message, char* c, bool b) {

        auto event = nes::make_holder<displayClientMessageEvent>(message);
        eventMgr.trigger(event);
        
        func(guidata, message, c, b);
    }

public:
    typedef void(__thiscall* original)(GuiData*, const std::string&, char*, bool);

    static inline original func = nullptr;

    displayClientMessageHook() : Hook("displayClientMessageHook", GET_SIG_ADDRESS("GuiData::displayClientMessage")) {}

    void enableHook() override {
        this->autoHook((void*)displayClientMessageDetour, (void**)&func);
    }
};