#pragma once

#include "../Hook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class OnSuspendHook : public Hook {

private:
    static void callback(void *a1, void *a2, void *a3, void *a4);

public:
    typedef void(__thiscall *original)(void *, void *, void *, void *);

    static inline original suspendOriginal = nullptr;

    OnSuspendHook() : Hook("onSuspendHook", GET_SIG("MinecraftGame::onAppSuspended")) {}

    void enableHook() override;
};