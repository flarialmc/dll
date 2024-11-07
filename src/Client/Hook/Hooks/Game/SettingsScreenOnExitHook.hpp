#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class SettingsScreenOnExitHook : public Hook {
private:
    static bool SettingsScreen_OnExit(void* _this) {
        auto pre_event = nes::make_holder<BeforeSettingsScreenOnExitEvent>();
        eventMgr.trigger(pre_event);

        bool state = funcOriginal(_this);

        auto post_event = nes::make_holder<AfterSettingsScreenOnExitEvent>();
        eventMgr.trigger(post_event);

        return state;
    }

public:
    typedef bool(__thiscall *original)(void*);

    static inline original funcOriginal = nullptr;

    SettingsScreenOnExitHook() : Hook("SettingsScreenOnExitHook", GET_SIG_ADDRESS("SettingsScreenOnExit")) {}

    void enableHook() override {
        this->autoHook((void *) SettingsScreen_OnExit, (void **) &funcOriginal);
    }
};