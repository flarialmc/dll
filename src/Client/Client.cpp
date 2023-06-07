#include "Client.hpp"

#include "../Utils/Logger/Logger.hpp"

void Client::initialize()
{
    HookManager::initialize();
    ModuleManager::initialize();

    Logger::initialize();
}
