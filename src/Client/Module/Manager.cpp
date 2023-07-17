#pragma once

#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/GUIKeyListener.hpp"
#include "Modules/Misc/Uninject/Uninject.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Module.hpp"
#include "Modules/FPS/FPSCounter.hpp"
#include "Modules/CPS/CPSCounter.hpp"
#include "Modules/Zoom/Zoom.hpp"
#include "Modules/Time/Time.hpp"
#include "Modules/Memory/Memory.hpp"
#include "Modules/Sprint/Sprint.hpp"
#include "Modules/Fullbright/Fullbright.hpp"
#include "Modules/Deepfry/Deepfry.hpp"
#include "Modules/ForceCoords/ForceCoords.hpp"
#include "Modules/Keystrokes/Keystrokes.hpp"
#include "Modules/ReachCounter/ReachCounter.hpp"
#include "Modules/ComboCounter/ComboCounter.hpp"
#include "Modules/IPDisplay/IPDisplay.hpp"
#include "Modules/PingCounter/PingCounter.hpp"
#include "Modules/PotCounter/PotCounter.hpp"
#include "Modules/ArrowCounter/ArrowCounter.hpp"
#include "Modules/SnapLook/SnapLook.hpp"
#include "Modules/Freelook/Freelook.hpp"
#include "Modules/ThirdPersonNametag/ThirdPerson.hpp"
#include "Modules/MotionBlur/MotionBlur.hpp"
#include "Modules/ArmorHUD/ArmorHUD.hpp"
#include "Modules/PatarHD/PatarHD.hpp"

namespace ModuleManager {
    std::vector<Module*> modules;
}

void ModuleManager::initialize()
{

    modules.push_back(new MotionBlur());
    modules.push_back(new Zoom());
    modules.push_back(new Deepfry());
    modules.push_back(new PatarHD());
    modules.push_back(new ClickGUI());
    modules.push_back(new FPSCounter());
    modules.push_back(new CPSCounter());
    modules.push_back(new IPDisplay());
    modules.push_back(new ReachCounter());
    modules.push_back(new ComboCounter());
    modules.push_back(new PingCounter());
    modules.push_back(new PotCounter());
    modules.push_back(new ArrowCounter());

    modules.push_back(new Time());
    modules.push_back(new MEM());
    modules.push_back(new Sprint());
    modules.push_back(new Fullbright());
    modules.push_back(new ForceCoords());
    modules.push_back(new Keystrokes());
    modules.push_back(new ThirdPerson());
    modules.push_back(new FreeLook());
    modules.push_back(new SnapLook());

    EventHandler::registerListener(new GUIKeyListener("E"));
    EventHandler::registerListener(new UninjectListener("Uninject"));
}

void ModuleManager::terminate()
{
    for (Module* module : modules) {
        module->onDisable();
    }

    modules.clear();
}

bool ModuleManager::doesAnyModuleHave(std::string settingName) {

    bool result = false;

    for (Module* mod : modules) {
        if(mod->settings.getSettingByName<bool>(settingName) != nullptr)
            if(mod->settings.getSettingByName<bool>(settingName)->value){
                result = true;
                break;
            }
    }

    return result;

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
