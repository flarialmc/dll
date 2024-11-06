#pragma once

#include "../Module.hpp"

class PackChanger : public Module {
private:
    bool patched = false;
    bool queueReset = false;
    bool canRender = false;
    bool enableFrameQueue = false;
    int frameQueue = 0;
public:
    PackChanger() : Module("PackChanger", "Allows you to change packs in world/server.", IDR_MAN_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        canRender = true;
        Listen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Listen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Listen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Listen(this, RenderCurrentFrameEvent, &PackChanger::onRenderCurrentFrame);
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Deafen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Deafen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Deafen(this, RenderCurrentFrameEvent, &PackChanger::onRenderCurrentFrame);
        canRender = true;
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
        canRender = false; // disable rendering
    }

    void onRenderCurrentFrame(RenderCurrentFrameEvent &event) {
        if(!canRender) event.cancel();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return unpatch();
        if(!player->getLevel()) return unpatch(); // means were not in the world
        patch();

        auto name = SDK::clientInstance->getTopScreenName();

        if(!canRender && enableFrameQueue) {
            if(frameQueue == 0) {
                canRender = true;
                enableFrameQueue = false;
            } else {
                if(frameQueue > 0) {
                    frameQueue--;
                }
            }
        }

        if(name == "pause_screen") {
            if (queueReset) {
                queueReset = false;
                SwapchainHook::queueReset = true;
                return;
            }
            if (!canRender && !SwapchainHook::queueReset && SwapchainHook::init) {
                if(!enableFrameQueue) {
                    enableFrameQueue = true;
                    frameQueue = 20;
                }
            }
        }
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