#pragma once
#include "Client.hpp"
#include "../Utils/Logger/Logger.hpp"
#include <kiero.h>

bool Client::disable = false;

void Client::initialize()
{
    Logger::initialize();


    if(kiero::init(kiero::RenderType::D3D12) != kiero::Status::Success || kiero::getRenderType() == kiero::RenderType::None)
    kiero::init(kiero::RenderType::D3D11);

    HookManager::initialize();
    ModuleManager::initialize();
}
