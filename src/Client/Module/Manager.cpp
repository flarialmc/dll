#pragma once
#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/TestListener.hpp"
#include "Modules/ClickGUI/GUIMouseListener.hpp"
#include "Modules/Misc/Uninject/Uninject.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

namespace ModuleManager {
    std::vector<Module*> modules;
}

void ModuleManager::initialize()
{
    // Test stuff
    EventHandler::registerListener(new TestListener("test"));

    modules.push_back(new Uninject());

}

void ModuleManager::terminate()
{
    for (Module* module : modules) {
        module->onDisable();
    }

    modules.clear();
}