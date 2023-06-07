#include "Client.hpp"

#include "../Utils/Logger/Logger.hpp"

void Client::initialize()
{
    Logger::initialize();

    HookManager::initialize();
    ModuleManager::initialize();
}
