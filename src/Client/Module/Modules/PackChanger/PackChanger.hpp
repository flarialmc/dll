#pragma once

#include "../Module.hpp"

class PackChanger : public Module {
private:
    bool patched = false;
    bool queueReset = false;
public:
    PackChanger() : Module("PackChanger", "Allows you to change packs in world/server.", IDR_MAN_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Listen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Listen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Deafen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Deafen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Module::onDisable();
        unpatch();
    }

    void defaultConfig() override {}

    void settingsRender(float settingsOffset) override {}

    void onPacksLoad(PacksLoadEvent &event) {
        if(!SDK::clientInstance) return;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;
        if(!player->getLevel()) return; // means were not in the world
        // recreate swapchain
        queueReset = true;
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return unpatch();
        if(!player->getLevel()) return unpatch(); // means were not in the world

        if(SDK::clientInstance->getTopScreenName() == "hud_screen") {
            if(queueReset) {
                queueReset = false;
                SwapchainHook::queueReset = true;
            }
        }

        patch();
    }

    void onIsPreGame(isPreGameEvent &event) {
        // auto name = SDK::clientInstance->getTopScreenName();
        // isPreGame = name == "screen_world_controls_and_settings - global_texture_pack_tab" || event.getState();
        event.setState(true);
    }

    void patch() {
        if(patched) return;
        patched = true;
        static auto dst = GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch1") + 1;
        Memory::patchBytes((void*)dst, (BYTE*)"\x85", 1);
    }

    void unpatch() {
        if(!patched) return;
        patched = false;
        static auto dst = GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch1") + 1;
        Memory::patchBytes((void*)dst, (BYTE*)"\x84", 1);
    }
};