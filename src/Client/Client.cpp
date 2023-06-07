#include "Client.hpp"

#include "../Utils/Logger/Logger.hpp"
#include <kiero.h>

void Client::initialize()
{
    Logger::initialize();

    kiero::init(kiero::RenderType::D3D12);
    HookManager::initialize();
    ModuleManager::initialize();
}
