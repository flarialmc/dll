#pragma once
#include "Client.hpp"
#include "../Utils/Logger/Logger.hpp"
#include <kiero.h>

bool Client::disable = false;

void Client::initialize()
{
    Logger::initialize();
    kiero::init(kiero::RenderType::D3D12);
    HookManager::initialize();
    ModuleManager::initialize();
}
