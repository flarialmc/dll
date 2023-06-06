#include "Client.h"

#include "../Utils/Logger.h"

void Client::initialize()
{
    HooksManager = new class HooksManager();
    FrontendManager = new class FrontendManager();
    ModulesManager = new class ModulesManager();

    Logger::initialize();
}

HooksManager Client::getHooksManager() const
{
    return *HooksManager;
}

FrontendManager Client::getFrontendManager() const
{
    return *FrontendManager;
}

ModulesManager Client::getModulesManager() const
{
    return *ModulesManager;
}
