#pragma once

#include "../Module.hpp"

class PackChanger : public Module {
private:
    bool queueReset = false;
    bool canRender = false;
    bool enableFrameQueue = false;
    int frameQueue = 0;
    bool userRequestedReload = false;

    bool forcePreGame = false;

    static inline std::array<std::byte, 6> patch1Data;
    static inline std::array<std::byte, 2> patch2Data;
    static inline std::array<std::byte, 2> patch3Data;
    static inline std::array<std::byte, 6> patch4Data;
public:
    PackChanger() : Module("PackChanger", "Allows you to change packs in world/server.", IDR_AUTORQ_PNG, "") {
        static auto src = GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch");
        static auto src1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");
        static auto src2 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_processPendingImports_Patch");
        static auto src3 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_setResourcePackStacks_Patch");

        Memory::copyBytes((void*)src, patch1Data.data(), patch1Data.size());
        Memory::copyBytes((void*)src1, patch2Data.data(), patch2Data.size());
        Memory::copyBytes((void*)src2, patch3Data.data(), patch3Data.size());
        Memory::copyBytes((void*)src3, patch4Data.data(), patch4Data.size());

        Module::setup();
    };

    void onEnable() override {
        patchComposeFullStack();
        canRender = true;
        Listen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Listen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Listen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Listen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute);
        Listen(this, RenderChunkCoordinatorPreRenderTickEvent, &PackChanger::onRenderChunkCoordinatorPreRenderTick);
        Listen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Listen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        Module::onEnable();
    }

    void onDisable() override {
        unpatchComposeFullStack();
        Deafen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Deafen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Deafen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Deafen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute);
        Deafen(this, RenderChunkCoordinatorPreRenderTickEvent, &PackChanger::onRenderChunkCoordinatorPreRenderTick);
        Deafen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Deafen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        canRender = true;
        Module::onDisable();
    }

    void defaultConfig() override {}

    void settingsRender(float settingsOffset) override {}

    void onBeforeSettingsScreenOnExit(BeforeSettingsScreenOnExitEvent &event) {
        if(!SDK::clientInstance) return;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return; // means were not in the world
        forcePreGame = true;
        userRequestedReload = true;
        patch();
    }

    void onAfterSettingsScreenOnExit(AfterSettingsScreenOnExitEvent &event) {
        if(!SDK::clientInstance) return;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return; // means were not in the world
        forcePreGame = false;
        unpatch();
    }

    void onPacksLoad(PacksLoadEvent &event) {
        if(!SDK::clientInstance) return;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;
        // recreate swapchain
        if(userRequestedReload) {
            queueReset = true;
            canRender = false; // disable rendering
            forcePreGame = false;
            userRequestedReload = false;
        }
    }

    void onRenderChunkCoordinatorPreRenderTick(RenderChunkCoordinatorPreRenderTickEvent &event) {
        // stops chunks from updating
        if(!canRender) event.cancel();
    }

    void onRenderOrderExecute(RenderOrderExecuteEvent &event) {
        // stops most 3D level rendering
        if(!canRender) event.cancel();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;

        auto name = SDK::clientInstance->getTopScreenName();

        if(!canRender && enableFrameQueue) {
            if(frameQueue == 0) {
                if(!queueReset)
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
                if(!enableFrameQueue) {
                    enableFrameQueue = true;
                    frameQueue = 20;
                }
                if(frameQueue == 0) {
                    queueReset = false;
                    SwapchainHook::queueReset = true;
                }
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
        if(!SDK::clientInstance) return;
        auto name = SDK::clientInstance->getScreenName();
        bool value = event.getState() || forcePreGame || name == "screen_world_controls_and_settings - global_texture_pack_tab";
        event.setState(value);
    }

    void patch() {
        static auto dst1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");
        static auto dst2 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_processPendingImports_Patch");
        static auto dst3 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_setResourcePackStacks_Patch");

        Memory::nopBytes((void*)dst1, 2);
        Memory::patchBytes((void*)dst2, (BYTE*)"\x90\xE9", 2);  // make it always jump
        Memory::nopBytes((void*)dst3, 6);
    }

    void unpatch() {
        static auto dst1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");
        static auto dst2 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_processPendingImports_Patch");
        static auto dst3 = GET_SIG_ADDRESS("GeneralSettingsScreenController::_setResourcePackStacks_Patch");


        Memory::patchBytes((void*)dst1, patch2Data.data(), patch2Data.size());
        Memory::patchBytes((void*)dst2, patch3Data.data(), patch3Data.size());
        Memory::patchBytes((void*)dst3, patch4Data.data(), patch4Data.size());
    }

    void patchComposeFullStack() {
        static auto dst = GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch");

        Memory::nopBytes((void*)dst, 6);
    }

    void unpatchComposeFullStack() {
        static auto dst = GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch");

        Memory::patchBytes((void*)dst, patch1Data.data(), patch1Data.size());
    }
};