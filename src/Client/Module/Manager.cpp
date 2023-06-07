#pragma once
#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/TestListener.hpp"
#include "../GUI/GUIMouseListener.hpp"
#include "Modules/Misc/Uninject/Uninject.hpp"

namespace ModuleManager {
    std::vector<Module*> modules;
}

void ModuleManager::initialize()
{
    // Test stuff
    EventHandler::registerListener(new TestListener("test"));
    EventHandler::registerListener(new GUIMouseListener("GUIMouse"));

    modules.push_back(new Uninject());


}

void ModuleManager::terminate()
{
    for (Module* module : modules) {
        module->onDisable();
    }

    modules.clear();
}