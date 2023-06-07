#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/TestListener.hpp"

void ModuleManager::initialize()
{
    EventHandler::registerListener(new TestListener("test"));
}

void ModuleManager::terminate()
{
}