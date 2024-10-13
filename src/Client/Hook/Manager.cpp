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
//#include "Hooks/Game/RenderItemGroup.hpp"
//#include "Hooks/Game/getCurrentSwingDuration.hpp"

std::vector<Hook *> HookManager::hooks;

std::string dxVersion[5] = {"Couldn't initialize", "DX9", "DX10", "DX11", "DX12"};

void HookManager::initialize() {

    //wouldnt be a bad idea to use a smart pointer for these like std::shared_ptr :)

    MH_Initialize();


    kiero::init(kiero::RenderType::D3D12);

    if (kiero::getRenderType() == 0) kiero::init(kiero::RenderType::D3D11);

    if (kiero::getRenderType() == 0) {
        kiero::init(kiero::RenderType::D3D10);
        Logger::debug("[Kiero] Trying d3d10");
    }

    Logger::debug(std::format("[Kiero] Renderer: {}", dxVersion[kiero::getRenderType()]));

    hooks.push_back(new KeyHook());
    hooks.push_back(new MouseHook());

    if (!Client::settings.getSettingByName<bool>("killdx")->value) hooks.push_back(new CommandListHook());

    hooks.push_back(new SwapchainHook());
    hooks.push_back(new ResizeHook());


    hooks.push_back(new TextureGroup_getTextureHook());
    hooks.push_back(new getViewPerspectiveHook());
    // hooks.push_back(new RenderActorHook());
    hooks.push_back(new RaknetTickHook());
    hooks.push_back(new SetUpAndRenderHook());
    hooks.push_back(new GameModeAttackHook());

    hooks.push_back(new getFovHook());
    hooks.push_back(new ActorBaseTick());
    hooks.push_back(new OnSuspendHook());
    hooks.push_back(new getGammaHook());
    hooks.push_back(new FontDrawTransformedHook());
    hooks.push_back(new HurtColorHook());
    hooks.push_back(new DimensionFogColorHook());
    hooks.push_back(new OverworldFogColorHook());
    hooks.push_back(new TimeChangerHook());
    hooks.push_back(new SendPacketHook());
    hooks.push_back(new getSensHook());
    hooks.push_back(new HudMobEffectsRendererHook());
    //hooks.push_back(new RenderItemGroupHook());
    //hooks.push_back(new getCurrentSwingDuration());

    for (auto hook: hooks)
        hook->enableHook();

}

void HookManager::terminate() {

    for (auto hook: hooks)
        delete hook;

    hooks.clear();
}

