#pragma once
#include "Client.hpp"
#include "../Utils/Logger/Logger.hpp"
#include <kiero.h>

bool Client::disable = false;

void Client::initialize()
{
    Logger::initialize();

    if(GetModuleHandle("d3d12.dll") != NULL)
    kiero::init(kiero::RenderType::D3D12);

    if (kiero::getRenderType() == kiero::RenderType::None)
    kiero::init(kiero::RenderType::D3D11);

    HookManager::initialize();
    ModuleManager::initialize();
}
