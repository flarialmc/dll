#pragma once
#include "Manager.hpp"
#include "Hooks/Render/ResizeHook.hpp"
#include "Hooks/Render/CommandListHook.hpp"
#include "Hooks/Render/SetupAndRenderHook.hpp"
#include "Hooks/Game/ActorNormalTick.hpp"
#include "Hooks/Visual/getGammaHook.hpp"
#include "Hooks/Game/GameModeAttack.hpp"
#include "Hooks/Game/RaknetTick.hpp"
#include "Hooks/Game/SendPacket.hpp"
#include "Hooks/Game/getViewPerspective.hpp"
#include "../Client.hpp"
#include "Hooks/Visual/HurtColorHook.hpp"
#include "Hooks/Visual/FogColorHook.hpp"
#include "Hooks/Visual/TimeChangerHook.hpp"

std::vector<Hook *> HookManager::hooks;

void HookManager::initialize()
{

    //wouldnt be a bad idea to use a smart pointer for these like std::shared_ptr :)

    MH_Initialize();


    kiero::init(kiero::RenderType::D3D12);

    if(kiero::getRenderType() == 0) kiero::init(kiero::RenderType::D3D11);

    Logger::debug(std::to_string(kiero::getRenderType()));

    hooks.push_back(new KeyHook());
    hooks.push_back(new MouseHook());

    if(!Client::settings.getSettingByName<bool>("killdx")->value) hooks.push_back(new CommandListHook());

    hooks.push_back(new getViewPerspectiveHook());
    hooks.push_back(new RaknetTickHook());
    hooks.push_back(new SetUpAndRenderHook());
    hooks.push_back(new GameModeAttackHook());
    hooks.push_back(new SwapchainHook());
    hooks.push_back(new ResizeHook());
    hooks.push_back(new getFovHook());
    hooks.push_back(new ActorNormalTick());
    hooks.push_back(new OnSuspendHook());
    hooks.push_back(new getGammaHook());
    hooks.push_back(new HurtColorHook());
    hooks.push_back(new FogColorHook());
    hooks.push_back(new TimeChangerHook());
    hooks.push_back(new SendPacketHook());

    for (auto hook : hooks)
        hook->enableHook();

}

void HookManager::terminate()
{

    for (auto hook : hooks)
        delete hook;

    hooks.clear();
}

