#pragma once
#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/GUIKeyListener.hpp"
#include "Modules/ClickGUI/GUIMouseListener.hpp"
#include "Modules/Misc/Uninject/Uninject.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Module.hpp"
#include "Modules/FPS/FPSCounter.hpp"
#include "Modules/Zoom/Zoom.hpp"


namespace ModuleManager {
    std::vector<Module*> modules;
}

void ModuleManager::initialize()
{


    modules.push_back(new Uninject());
    modules.push_back(new ClickGUI());
    modules.push_back(new Zoom());
    modules.push_back(new FPSCounter());
    modules.push_back(new Module("test", "woah", "", 1));
    modules.push_back(new Module("test", "woah", "", 1));
    modules.push_back(new Module("test", "woah", "", 1));
    modules.push_back(new Module("test", "woah", "", 1));
    modules.push_back(new Module("test", "woah", "", 1));
    modules.push_back(new Module("test", "woah", "", 1));
    EventHandler::registerListener(new GUIKeyListener("E"));


}

void ModuleManager::terminate()
{
    for (Module* module : modules) {
        module->onDisable();
    }

    modules.clear();
}

Module* ModuleManager::getModule(std::string name)
{
    for (Module* mod : modules) {
        if (mod->name == name) {
            return mod;

        }
    }

    return nullptr;
}
