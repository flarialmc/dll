#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class isPreGameHook : public Hook {
private:
    static bool ClientInstance_isPreGame(ClientInstance* _this) {
        bool state = funcOriginal(_this);

        auto event = nes::make_holder<isPreGameEvent>(state);
        eventMgr.trigger(event);

        return event->getState();
    }

public:
    typedef bool(__thiscall *original)(ClientInstance*);

    static inline original funcOriginal = nullptr;

    isPreGameHook() : Hook("isPreGameHook", GET_SIG_ADDRESS("ClientInstance::isPreGame")) {}

    void enableHook() override {
        this->autoHook((void *) ClientInstance_isPreGame, (void **) &funcOriginal);
    }
};