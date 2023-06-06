#include "Client.hpp"

#include "../Utils/Logger/Logger.hpp"

void Client::initialize()
{
    HooksManager = new class HooksManager();
    ModulesManager = new class ModulesManager();

    Logger::initialize();
}

HooksManager Client::getHooksManager() const
{
    return *HooksManager;
}

ModulesManager Client::getModulesManager() const
{
    return *ModulesManager;
}
