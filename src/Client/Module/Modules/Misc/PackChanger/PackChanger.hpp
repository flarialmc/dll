#pragma once

#include "src/Client/Module/Modules/Module.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class PackChanger : public Listener {
private:
    bool recreate = false;
    bool canRender = false;
    bool enableFrameQueue = false;
    int frameQueue = 0;
    bool userRequestedReload = false;

    bool forcePreGame = false;
    bool canUseKeys = false;

    std::string last_tab = "";

    static inline std::array<std::byte, 6> patch1Data;
    static inline std::array<std::byte, 2> patch2Data;
public:
    PackChanger() {
        if(VersionUtils::checkAboveOrEqual(21,00)) {
            static auto src = VersionUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");
            Memory::copyBytes((void*)src, patch1Data.data(), patch1Data.size());
        }

        static auto src1 = GET_SIG_ADDRESS("SettingsScreenOnExit_Patch");
        Memory::copyBytes((void*)src1, patch2Data.data(), patch2Data.size());

        Init();
    };

    ~PackChanger() {
        Deactivate();
    }

    void Init() {
        patchComposeFullStack();
        canRender = true;
        canUseKeys = true;
        Listen(this, KeyEvent, &PackChanger::onKey);
        Listen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Listen(this, isPreGameEvent, &PackChanger::onIsPreGame); // unlocking buttons
        Listen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Listen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute); // stops most 3D level rendering
        Listen(this, HandleVisibilityUpdatesEvent, &PackChanger::onHandleVisibilityUpdates); // stops chunks from re-generating
        Listen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Listen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        Listen(this, GeneralSettingsScreenControllerOnCreateEvent, &PackChanger::onGeneralSettingsScreenControllerOnCreate);
    }

    void Deactivate() {
        unpatchComposeFullStack();
        Deafen(this, KeyEvent, &PackChanger::onKey);
        Deafen(this, SetupAndRenderEvent, &PackChanger::onSetupAndRender);
        Deafen(this, isPreGameEvent, &PackChanger::onIsPreGame);
        Deafen(this, PacksLoadEvent, &PackChanger::onPacksLoad);
        Deafen(this, RenderOrderExecuteEvent, &PackChanger::onRenderOrderExecute);
        Deafen(this, HandleVisibilityUpdatesEvent, &PackChanger::onHandleVisibilityUpdates);
        Deafen(this, BeforeSettingsScreenOnExitEvent, &PackChanger::onBeforeSettingsScreenOnExit);
        Deafen(this, AfterSettingsScreenOnExitEvent, &PackChanger::onAfterSettingsScreenOnExit);
        Deafen(this, GeneralSettingsScreenControllerOnCreateEvent, &PackChanger::onGeneralSettingsScreenControllerOnCreate);
        canRender = true;
        canUseKeys = true;
    }

    void onKey(KeyEvent &event) {
        if(!canUseKeys) {
            event.cancel(); // prevent users from closing pack loading menu
        }
    }

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
            recreate = true;
            forcePreGame = false;
            userRequestedReload = false;
        }
    }

    void onHandleVisibilityUpdates(HandleVisibilityUpdatesEvent &event) {
        // stops chunks from updating
        //Logger::debug("canRender: {}, recreate: {}, forcePreGame: {}", canRender, recreate, forcePreGame);
        if(!canRender || recreate || forcePreGame) event.cancel();
    }

    void onRenderOrderExecute(RenderOrderExecuteEvent &event) {
        // stops most 3D level rendering
        //Logger::debug("canRender: {}, recreate: {}, forcePreGame: {}", canRender, recreate, forcePreGame);
        if(!canRender || recreate || forcePreGame) event.cancel();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;

        auto name = SDK::clientInstance->getScreenName();

        if (frameQueue > 0) frameQueue--;
        if(!canRender && enableFrameQueue) {
            if(frameQueue == 0) {
                if(!recreate) {
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
            if (recreate) {
                if(!enableFrameQueue) {
                    enableFrameQueue = true;
                    frameQueue = 2;
                }
                if(frameQueue == 0) {
                    recreate = false;

                    SDK::clientInstance->getLevelRender()->getLevelRendererPlayer()->onDeviceLost();
                    SDK::clientInstance->getMinecraftGame()->_onResumeWaitReloadActors();
                    //SwapchainHook::recreate = true;
                }
                return;
            }
            if (!canRender && !SwapchainHook::recreate && SwapchainHook::init) {
                if(!enableFrameQueue) {
                    enableFrameQueue = true;
                    frameQueue = 2;
                }
            }
        }
    }

    void onIsPreGame(isPreGameEvent &event) {
        if(!SDK::clientInstance) return;
        auto name = SDK::clientInstance->getScreenName();
        auto topScreenName = SDK::clientInstance->getTopScreenName();
        if(name.find("screen_world_controls_and_settings - ") != std::string::npos) {
            last_tab = name;
        };
        static std::string tab = "screen_world_controls_and_settings";
        bool value = event.getState() || forcePreGame || name == tab || last_tab == tab && name == "screen";
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
        if(!VersionUtils::checkAboveOrEqual(21,00)) return;

        static auto dst = VersionUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");

        Memory::nopBytes((void*)dst, 6);
    }

    void unpatchComposeFullStack() {
        if(!VersionUtils::checkAboveOrEqual(21,00)) return;

        static auto dst = VersionUtils::checkAboveOrEqual(21,20) ? GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack_Patch") : GET_SIG_ADDRESS("MinecraftGame::_onActiveResourcePacksChanged_Patch");

        Memory::patchBytes((void*)dst, patch1Data.data(), patch1Data.size());
    }
};