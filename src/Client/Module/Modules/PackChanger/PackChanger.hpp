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
    bool canUseKeys = false;

    static inline std::array<std::byte, 6> patch1Data;
    static inline std::array<std::byte, 2> patch2Data;
public:
    PackChanger() : Module("PackChanger", "Allows you to change packs in world/server.", IDR_AUTORQ_PNG, "") {

        if(WinrtUtils::checkAboveOrEqual(21,00)) {
            static auto src = WinrtUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");
            Memory::copyBytes((void*)src, patch1Data.data(), patch1Data.size());
        }

        static auto src1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");
        Memory::copyBytes((void*)src1, patch2Data.data(), patch2Data.size());

        Module::setup();
    };

    void onEnable() override {
        patchComposeFullStack();
        canRender = true;
        canUseKeys = true;
        Listen(this, KeyEvent, &PackChanger::onKey);
        Listen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Listen(this, isPreGameEvent, &PackChanger::onIsPreGame); // unlocking buttons
        Listen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Listen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute); // stops most 3D level rendering
        Listen(this, RenderChunkCoordinatorPreRenderTickEvent, &PackChanger::onRenderChunkCoordinatorPreRenderTick); // stops chunks from re-generating
        Listen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Listen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        Listen(this, GeneralSettingsScreenControllerOnCreateEvent, &PackChanger::onGeneralSettingsScreenControllerOnCreate);
        Module::onEnable();
    }

    void onDisable() override {
        unpatchComposeFullStack();
        Deafen(this, KeyEvent, &PackChanger::onKey);
        Deafen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Deafen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Deafen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Deafen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute);
        Deafen(this, RenderChunkCoordinatorPreRenderTickEvent, &PackChanger::onRenderChunkCoordinatorPreRenderTick);
        Deafen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Deafen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        Deafen(this, GeneralSettingsScreenControllerOnCreateEvent, &PackChanger::onGeneralSettingsScreenControllerOnCreate);
        canRender = true;
        canUseKeys = true;
        Module::onDisable();
    }

    void onKey(KeyEvent &event) {
        if(!canUseKeys) {
            event.cancel(); // prevent users from closing pack loading menu
        }
    }

    void defaultConfig() override {}

    void settingsRender(float settingsOffset) override {}

    void onGeneralSettingsScreenControllerOnCreate(GeneralSettingsScreenControllerOnCreateEvent &event) {
        event.unlockPackMenu();
    }

    void onBeforeSettingsScreenOnExit(BeforeSettingsScreenOnExitEvent &event) {
        if(!SDK::clientInstance) return;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return; // means were not in the world
        forcePreGame = true;
        canRender = false; // disable rendering
        userRequestedReload = true;
        canUseKeys = false;
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
            forcePreGame = false;
            userRequestedReload = false;
        }
    }

    void onRenderChunkCoordinatorPreRenderTick(RenderChunkCoordinatorPreRenderTickEvent &event) {
        // stops chunks from updating
        if(!canRender || queueReset || forcePreGame) event.cancel();
    }

    void onRenderOrderExecute(RenderOrderExecuteEvent &event) {
        // stops most 3D level rendering
        if(!canRender || queueReset || forcePreGame) event.cancel();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;

        auto name = SDK::clientInstance->getScreenName();

        if(!canRender && enableFrameQueue) {
            if(frameQueue == 0) {
                if(!queueReset) {
                    forcePreGame = false;
                    canRender = true;
                    canUseKeys = true;
                }
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

                    SDK::clientInstance->getLevelRender()->getLevelRendererPlayer()->onDeviceLost();
                    SDK::clientInstance->getMinecraftGame()->_onResumeWaitReloadActors();
                    //SwapchainHook::queueReset = true;
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

        Memory::nopBytes((void*)dst1, 2);
    }

    void unpatch() {
        static auto dst1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");

        Memory::patchBytes((void*)dst1, patch2Data.data(), patch2Data.size());
    }

    void patchComposeFullStack() {
        if(!WinrtUtils::checkAboveOrEqual(21,00)) return;

        static auto dst = WinrtUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");

        Memory::nopBytes((void*)dst, 6);
    }

    void unpatchComposeFullStack() {
        if(!WinrtUtils::checkAboveOrEqual(21,00)) return;

        static auto dst = WinrtUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");

        Memory::patchBytes((void*)dst, patch1Data.data(), patch1Data.size());
    }
};