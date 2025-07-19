#pragma once

#include "Hooks/Render/ResizeHook.hpp"
#include "Hooks/Render/CommandListHook.hpp"
#include "Hooks/Render/SetupAndRenderHook.hpp"
#include "Hooks/Game/ActorBaseTick.hpp"
#include "Hooks/Visual/getGammaHook.hpp"
#include "Hooks/Game/GameModeAttack.hpp"
#include "Hooks/Game/RaknetTick.hpp"
#include "Hooks/Game/PacketHooks.hpp"
#include "Hooks/Game/getViewPerspective.hpp"
#include "Hooks/Visual/FontDrawTransformedHook.hpp"
#include "Hooks/Visual/HurtColorHook.hpp"
#include "Hooks/Visual/DimensionFogColorHook.hpp"
#include "Hooks/Visual/OverworldFogColorHook.hpp"
#include "Hooks/Visual/TimeChangerHook.hpp"
#include "Hooks/Game/getSensHook.hpp"
#include "Hooks/Game/ContainerScreenControllerHook.hpp"
#include "Hooks/Render/TextureGroup_getTextureHook.hpp"
#include "Hooks/Render/HudMobEffectsRenderer.hpp"
#include "Hooks/Visual/BaseActorRendererRenderTextHook.hpp"
#include "Hooks/Game/UpdatePlayerHook.hpp"
#include "Hooks/Game/isPreGame.hpp"
#include "Hooks/Game/composeFullStack.hpp"
#include "Hooks/Render/RenderOrderExecuteHook.hpp"
#include "Hooks/Render/RenderChunkCoordinatorHandleVisibilityUpdatesHook.hpp"
#include "Hooks/Game/SettingsScreenOnExitHook.hpp"
#include "Hooks/Render/UIControl_updateCachedPositionHook.hpp"
#include "Hooks/Render/HudCursorRenderer.hpp"
#include "Hooks/Render/GeneralSettingsScreenControllerCtorHook.hpp"
#include "Hooks/Render/TickingTextureStageRenderHook.hpp"
//#include "Hooks/Game/RenderItemGroup.hpp"
//#include "Hooks/Game/getCurrentSwingDuration.hpp"
#include "Hooks/Game/ItemInHandRendererRenderItem.hpp"
#include "Hooks/Visual/RenderOutlineSelectionHook.hpp"
#include "Hooks/Game/displayClientMessage.hpp"
#include "Hooks/Game/getTimeOfDayHook.hpp"
#include "Hooks/game/ReadFileHook.hpp"
#include "Hooks/Game/ApplyTurnDeltaHook.hpp"

#include "Hooks/Render/BobHurt.hpp"
#include "Hooks/Visual/ActorShaderParams.hpp"
#include "Hooks/Visual/TintColorHook.hpp"
#include "Hooks/Render/RenderLevelHook.hpp"

std::vector<std::shared_ptr<Hook>> HookManager::hooks;

std::string dxVersion[5] = {"Couldn't initialize", "DX9", "DX10", "DX11", "DX12"};

void HookManager::initialize() {
    uint64_t start = Utils::getCurrentMs();
    MH_Initialize();

    kiero::init(kiero::RenderType::D3D12);

    if (kiero::getRenderType() == 0) kiero::init(kiero::RenderType::D3D11);

    if (kiero::getRenderType() == 0) {
        kiero::init(kiero::RenderType::D3D10);
        Logger::debug("[Kiero] Trying d3d10");
    }

    Logger::debug("Renderer: {}", dxVersion[kiero::getRenderType()]);


    addHook<KeyHook>();
    addHook<MouseHook>();

    if (!Client::settings.getSettingByName<bool>("killdx")->value)
        addHook<CommandListHook>();

    addHook<SwapchainHook>();
    addHook<ResizeHook>();

    if(VersionUtils::checkAboveOrEqual(21, 40)) {
        addHook<TextureGroup_getTextureHook>();
    }
    addHook<getViewPerspectiveHook>();
    addHook<RaknetTickHook>();
    addHook<SetUpAndRenderHook>();
    addHook<GameModeAttackHook>();

    addHook<getFovHook>();
    addHook<displayClientMessageHook>();
    addHook<ActorBaseTick>();
    addHook<OnSuspendHook>();
    addHook<getGammaHook>();
    addHook<FontDrawTransformedHook>();
    addHook<HurtColorHook>();
    addHook<DimensionFogColorHook>();
    addHook<OverworldFogColorHook>();
    addHook<TimeChangerHook>();
    addHook<SendPacketHook>();
    addHook<ApplyTurnDeltaHook>();
    //addHook<getSensHook>();
    addHook<HudMobEffectsRendererHook>();
    if(VersionUtils::checkAboveOrEqual(20, 60)) { // due to texture group offset
        addHook<HudCursorRendererHook>();
        addHook<BaseActorRendererRenderTextHook>();

        addHook<TickingTextureStageRenderHook>(); // due to mv
    }
    addHook<UIControl_updateCachedPositionHook>();
    addHook<GeneralSettingsScreenControllerCtorHook>();

    if (VersionUtils::checkAboveOrEqual(21, 40)) {
        addHook<ContainerScreenControllerHook>();
    }

    if(!VersionUtils::checkAboveOrEqual(21, 80)) { //needed for MaterialBinLoader/shader loader
        addHook<_composeFullStackHook>();
    }

    // likely packchanger hooks, im not sure!
    if(!VersionUtils::checkAboveOrEqual(21, 60))
    {
        addHook<isPreGameHook>();

        addHook<RenderOrderExecuteHook>();
        addHook<RenderChunkCoordinatorHandleVisibilityUpdatesHook>();
//
        addHook<SettingsScreenOnExitHook>();
    }

    addHook<ItemInHandRendererRenderItem>();

    addHook<RenderOutlineSelectionHook>();
    addHook<getTimeOfDayHook>();

    addHook<BobHurtHook>();
    addHook<TintColorHook>();
    addHook<ActorShaderParamsHook>();
    addHook<RenderLevelHook>();

    if(VersionUtils::checkAboveOrEqual(21, 40)) {
        addHook<UpdatePlayerHook>();
    }

    if(VersionUtils::checkAboveOrEqual(21, 50)) {
        addHook<ReadFileHook>();
    }
    for (const auto& hook: hooks) {
        hook->enableHook();
    }

    float elapsed = (Utils::getCurrentMs() - start) / 1000.0;
    Logger::custom(fg(fmt::color::deep_sky_blue), "Hook", "Initialized {} hooks in {:.2f}s", hooks.size(), elapsed);
}

void HookManager::terminate() {
    hooks.clear();
}

