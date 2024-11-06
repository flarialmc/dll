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
#include "Hooks/Render/TextureGroup_getTextureHook.hpp"
#include "Hooks/Render/HudMobEffectsRenderer.hpp"
#include "Hooks/Visual/BaseActorRendererRenderTextHook.hpp"
#include "Hooks/Game/UpdatePlayerHook.hpp"
#include "Hooks/Game/isPreGame.hpp"
#include "Hooks/Game/composeFullStack.hpp"
#include "Hooks/Render/RenderCurrentFrame.hpp"
#include "Hooks/Render/RebuildChunkHook.hpp"
//#include "Hooks/Game/RenderItemGroup.hpp"
//#include "Hooks/Game/getCurrentSwingDuration.hpp"

std::vector<std::shared_ptr<Hook>> HookManager::hooks;

std::string dxVersion[5] = {"Couldn't initialize", "DX9", "DX10", "DX11", "DX12"};

void HookManager::initialize() {
    MH_Initialize();

    kiero::init(kiero::RenderType::D3D12);

    if (kiero::getRenderType() == 0) kiero::init(kiero::RenderType::D3D11);

    if (kiero::getRenderType() == 0) {
        kiero::init(kiero::RenderType::D3D10);
        Logger::debug("[Kiero] Trying d3d10");
    }

    Logger::debug(std::format("[Kiero] Renderer: {}", dxVersion[kiero::getRenderType()]));


    addHook<KeyHook>();
    addHook<MouseHook>();

    if (!Client::settings.getSettingByName<bool>("killdx")->value)
        addHook<CommandListHook>();

    addHook<SwapchainHook>();
    addHook<ResizeHook>();

    //addHook<TextureGroup_getTextureHook>();
    addHook<getViewPerspectiveHook>();
    addHook<RaknetTickHook>();
    addHook<SetUpAndRenderHook>();
    addHook<GameModeAttackHook>();

    addHook<getFovHook>();
    addHook<ActorBaseTick>();
    addHook<OnSuspendHook>();
    addHook<getGammaHook>();
    addHook<FontDrawTransformedHook>();
    addHook<HurtColorHook>();
    addHook<DimensionFogColorHook>();
    addHook<OverworldFogColorHook>();
    addHook<TimeChangerHook>();
    addHook<SendPacketHook>();
    addHook<getSensHook>();
    addHook<HudMobEffectsRendererHook>();
    if(!WinrtUtils::checkAboveOrEqual(21, 40)) {
        addHook<BaseActorRendererRenderTextHook>();
    }
    if(WinrtUtils::checkAboveOrEqual(21, 40)) {
        addHook<UpdatePlayerHook>();
        addHook<isPreGameHook>();
        addHook<_composeFullStackHook>();

        addHook<RenderCurrentFrameHook>();
        addHook<RebuildChunkHook>();
    }

    for (const auto& hook: hooks)
        hook->enableHook();

}

void HookManager::terminate() {
    hooks.clear();
}

