#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/TestListener.hpp"
#include "../GUI/GUIMouseListener.hpp"

void ModuleManager::initialize()
{
    EventHandler::registerListener(new TestListener("test"));
    EventHandler::registerListener(new GUIMouseListener("GUIMouse"))
}

void ModuleManager::terminate()
{
}