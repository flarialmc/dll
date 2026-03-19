#pragma once

#include "Manager.hpp"
#include "Hooks/Input/KeyHook.hpp"
#include "Hooks/Input/MouseHook.hpp"
#include "Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

#include "Hooks/Render/DirectX/DXGI/ResizeHook.hpp"
#include "Hooks/Render/DirectX/DXGI/CommandListHook.hpp"
#include "Hooks/Render/SetupAndRenderHook.hpp"
#include "Hooks/Game/ActorBaseTick.hpp"
#include "Hooks/Visual/getGammaHook.hpp"
#include "Hooks/Game/GameModeAttack.hpp"
#include "Hooks/Game/RaknetTick.hpp"
#include "Hooks/Game/PacketHooks.hpp"
#include "Hooks/Game/getViewPerspective.hpp"
#include "Hooks/Visual/FontDrawTransformedHook.hpp"
#include "Hooks/Visual/HurtColorHook.hpp"
#include "Hooks/Visual/ColorFormatHook.hpp"
#include "Hooks/Visual/DimensionFogColorHook.hpp"
#include "Hooks/Visual/OverworldFogColorHook.hpp"
#include "Hooks/Visual/TimeChangerHook.hpp"
#include "Hooks/Render/ItemRendererRenderGroupHook.hpp"
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
#include "Hooks/Game/ChatScreenControllerHook.hpp"
#include "Hooks/Game/HudScreenControllerHook.hpp"
#include "Hooks/Game/SoundEnginePlayHook.hpp"
#include "Hooks/Game/BedrockLogHook.hpp"
#include "Hooks/Game/getCurrentSwingDurationHook.hpp"

#include "Hooks/Render/BobHurt.hpp"
#include "Hooks/Render/RenderLevelHook.hpp"
#include "Hooks/Visual/ActorShaderParams.hpp"
#include "Hooks/Visual/TintColorHook.hpp"
#include "Hooks/Visual/Level_addParticleEffect.hpp"
#include "Hooks/Visual/Level_sendServerLegacyParticle.hpp"
#include "Hooks/Game/ActorDropItem.hpp"
#include "Hooks/Game/InventoryAddItem.hpp"
#include "Hooks/Game/AnimationComponent_playAnimation.hpp"
#include "Hooks/Game/SplashTextHook.hpp"
#include "Hooks/Game/UnicodeWndProcHack.hpp"
#include "Hooks/Input/ClipCursorHook.hpp"
#include "Hooks/Input/CursorHandler.hpp"
#include "Hooks/Input/TabOutFix.hpp"
#include "Hooks/Render/UpdateCameraHook.hpp"
#include "Utils/WinrtUtils.hpp"
#include "Hooks/Render/BgfxFrameExtractorInsertHook.hpp"
#include "Hooks/Game/ClientInstanceUpdate.hpp"
#include "Hooks/Game/ParseThirdPartyServersHook.hpp"
#include "Hooks/Render/BoneTransformHook.hpp"

std::vector<std::shared_ptr<Hook>> HookManager::hooks;

std::string dxVersion[5] = {"Couldn't initialize", "DX9", "DX10", "DX11", "DX12"};

void HookManager::initialize()
{
    uint64_t start = Utils::getCurrentMs();
    MH_Initialize();

    kiero::init(kiero::RenderType::D3D12);

    if (kiero::getRenderType() == 0) kiero::init(kiero::RenderType::D3D11);

    if (kiero::getRenderType() == 0)
    {
        kiero::init(kiero::RenderType::D3D10);
        Logger::debug("[Kiero] Trying d3d10");
    }

    if (VersionUtils::checkAboveOrEqual(21, 120))
    {
        addHook<CursorHandler>();
        addHook<UnicodeWndProcHack>();
    }

    if (VersionUtils::checkEqual(21, 121))
        addHook<TabOutFixHook>();
    if (VersionUtils::checkAboveOrEqual(21, 130))
    {
        addHook<UpdateCameraHook>();
    }

  //  addHook<ParseThirdPartyServersHook>();
    addHook<KeyHook>();
    addHook<MouseHook>();

    if(VersionUtils::checkAboveOrEqual(21, 130)) {
        addHook<ClientInstanceUpdateHook>();
    }

    // Always add CommandListHook - we need it to capture the DX12 command queue
    // even when killdx is enabled, because the DX11 fallback might not work on GDK
    addHook<CommandListHook>();

    addHook<SwapchainHook>();
    addHook<ResizeHook>();

    if (VersionUtils::checkAboveOrEqual(21, 40))
    {
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
    addHook<OnDeviceLostHook>();
    addHook<getGammaHook>();
    if (!VersionUtils::checkAboveOrEqual(21, 120)) addHook<FontDrawTransformedHook>();
    if (VersionUtils::checkAboveOrEqual(21, 130)) addHook<HurtColorHook>();
    addHook<ColorFormatHook>();
    addHook<DimensionFogColorHook>();
    addHook<OverworldFogColorHook>();
    addHook<TimeChangerHook>();
    addHook<ItemRendererRenderGroupHook>();
    addHook<SendPacketHook>();
    addHook<ApplyTurnDeltaHook>();
    //addHook<AnimationComponent_playAnimation>();
    if (VersionUtils::checkBelow(21, 80)) {
        addHook<getSensHook>(); // Sensitivity is now handled in ApplyTurnDeltaHook (for .80+)
    }
    addHook<HudMobEffectsRendererHook>();
    if (VersionUtils::checkAboveOrEqual(20, 60))
    {
        // due to texture group offset
        addHook<HudCursorRendererHook>();
        addHook<BaseActorRendererRenderTextHook>();

        addHook<TickingTextureStageRenderHook>(); // due to mv
    }
    addHook<UIControl_updateCachedPositionHook>();

    if (VersionUtils::checkAboveOrEqual(21, 40))
    {
        addHook<ContainerScreenControllerHook>();
    }

    addHook<_composeFullStackHook>();

    // packchanger hooks
    if (!VersionUtils::checkAboveOrEqual(21, 60))
    {
        addHook<isPreGameHook>();

        addHook<RenderOrderExecuteHook>();
        addHook<RenderChunkCoordinatorHandleVisibilityUpdatesHook>();
        addHook<SettingsScreenOnExitHook>();
        addHook<GeneralSettingsScreenControllerCtorHook>();
    }

        addHook<ItemInHandRendererRenderItem>();

    addHook<RenderOutlineSelectionHook>();
    addHook<getTimeOfDayHook>();

    addHook<BobHurtHook>();
    addHook<RenderLevelHook>();
    addHook<TintColorHook>();
    addHook<ActorShaderParamsHook>();
    addHook<ChatScreenControllerHook>();
    addHook<HudScreenControllerHook>();
    //addHook<Level_addParticleEffect>();
    //addHook<Level_sendServerLegacyParticle>();
    addHook<SoundEnginePlayHook>();
    addHook<getCurrentSwingDurationHook>();
    addHook<ActorDropItem>();
    addHook<InventoryAddItem>();
    if (VersionUtils::checkAboveOrEqual(21, 40))
    {
        addHook<UpdatePlayerHook>();
    }

    if (VersionUtils::checkAboveOrEqual(21, 50))
    {
        addHook<ReadFileHook>();
    }

    // Better Inventory hooks (1.21.130+)
    if (VersionUtils::checkAboveOrEqual(21, 130))
    {
        // TODO: Re-enable when hook classes are restored in BetterInventory port refactor.
        // addHook<FlyingItemRendererHook>();
        // addHook<IconBlitMultiColorHook>();
    }

    if (VersionUtils::checkAboveOrEqual(21, 130)) {
        addHook<BgfxFrameExtractorInsertHook>();
        addHook<BoneTransformHook>();
    }

#ifdef __DEBUG__
    if (VersionUtils::checkAboveOrEqual(21, 130)) addHook<BedrockLogHook>();
#endif

    // if (VersionUtils::checkEqual(21, 130)) addHook<SplashTextHook>();
    for (const auto& hook : hooks)
    {
        hook->enableHook();
    }

    float elapsed = (Utils::getCurrentMs() - start) / 1000.0;
    Logger::custom(fg(fmt::color::deep_sky_blue), "Hook", "Initialized {} hooks in {:.2f}s",
                   hooks.size(), elapsed);
}

void HookManager::terminate()
{
    hooks.clear();
}
