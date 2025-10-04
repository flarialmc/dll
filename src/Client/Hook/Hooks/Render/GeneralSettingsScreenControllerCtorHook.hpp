#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class GeneralSettingsScreenControllerCtorHook : public Hook {
private:
    static void* GeneralSettingsScreenController__GeneralSettingsScreenController(void* _this, void* model, bool isGlobalSettings, bool reloadTexturePacksOnExit) {
        auto event = nes::make_holder<GeneralSettingsScreenControllerOnCreateEvent>(isGlobalSettings, reloadTexturePacksOnExit);
        eventMgr.trigger(event);
        return funcOriginal(_this, model, event->getIsGlobalSettings(), event->getReloadTexturePacksOnExit());
    }

public:
    typedef void*(__thiscall *original)(void *_this, void* model, bool isGlobalSettings, bool reloadTexturePacksOnExit);

    static inline original funcOriginal = nullptr;

    GeneralSettingsScreenControllerCtorHook() : Hook("GeneralSettingsScreenController::GeneralSettingsScreenControllerHook", GET_SIG_ADDRESS("GeneralSettingsScreenController::GeneralSettingsScreenController")) {}

    void enableHook() override {
        this->autoHook((void *) GeneralSettingsScreenController__GeneralSettingsScreenController, (void **) &funcOriginal);
    }
};