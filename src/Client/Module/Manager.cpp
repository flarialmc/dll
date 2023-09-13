#pragma once

#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/GUIKeyListener.hpp"
#include "Modules/Misc/Uninject/Uninject.hpp"
#include "Modules/Misc/SaveConfig/SaveConfig.hpp"
#include "Modules/Misc/RGB/rgb.hpp"
#include "Modules/Misc/TextAlias/TextAlias.hpp"
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
#include "Modules/PaperDoll/PaperDoll.hpp"
#include "Modules/PatarHD/PatarHD.hpp"
#include "Modules/HurtColor/HurtColor.hpp"
#include "Modules/FogColor/FogColor.hpp"
#include "Modules/TimeChanger/TimeChanger.hpp"
#include "Modules/RenderOptions/RenderOptions.hpp"
#include "Modules/HueChanger/HueChanger.hpp"
#include "Modules/Sneak/Sneak.hpp"
#include "Modules/GuiScale/GuiScale.hpp"
#include "Modules/WeatherChanger/WeatherChanger.hpp"
#include "Modules/TabList/TabList.hpp"
#include "Modules/AutoGG/AutoGG.hpp"
#include "Modules/TextHotkey/TextHotkey.hpp"
#include "Modules/Nick/NickModule.hpp"
#include "Modules/SpeedDisplay/SpeedDisplay.hpp"
#include "Modules/Misc/CentreCursor/CentreCursor.hpp"
#include "Modules/CPSLimiter/CPSLimiter.hpp"
#include "Modules/BlockBreakIndicator/BlockBreakIndicator.hpp"
#include "Modules/CompactChat/CompactChat.hpp"
#include "Modules/MovableChat/MovableChat.hpp"
#include "Modules/FOVChanger/FOVChanger.hpp"
#include "Modules/UpsideDown/UpsideDown.hpp"
#include "Modules/Animations/Animations.hpp"
#include <algorithm>

bool compareNames( Module*& obj1,  Module*& obj2) {
    return obj1->name < obj2->name;
}

namespace ModuleManager {
    std::vector<Module*> modules;
}

void ModuleManager::initialize()
{

    modules.push_back(new MotionBlur());
    modules.push_back(new Zoom());
    modules.push_back(new Deepfry());
    modules.push_back(new HueChanger());
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
    modules.push_back(new SnapLook());
    modules.push_back(new HurtColor());
    modules.push_back(new FogColor());
    modules.push_back(new ArmorHUD());
    modules.push_back(new TimeChanger());
    modules.push_back(new RenderOptions());
    modules.push_back(new PaperDoll());
    modules.push_back(new Sneak());
    modules.push_back(new GuiScale());
    modules.push_back(new WeatherChanger());
    modules.push_back(new TabList());
    modules.push_back(new AutoGG());
    modules.push_back(new TextHotkey());
    modules.push_back(new NickModule());
    modules.push_back(new FreeLook());
    modules.push_back(new SpeedDisplay());
    modules.push_back(new CPSLimiter());
    modules.push_back(new BlockBreakIndicator());
    //modules.push_back(new MovableChat());
    modules.push_back(new FOVChanger());
    //modules.push_back(new CompactChat());
    modules.push_back(new UpsideDown());
    modules.push_back(new Animations());

    std::sort(modules.begin(), modules.end(), compareNames);

    EventHandler::registerListener(new GUIKeyListener("E"));
    EventHandler::registerListener(new UninjectListener("Uninject"));
    EventHandler::registerListener(new SaveConfigListener("SaveConfig"));
    EventHandler::registerListener(new CentreCursorListener("CentreCursor"));
    EventHandler::registerListener(new rgbListener("RGB Controller"));
    EventHandler::registerListener(new TextAliasListener("TextAlias"));
}

void ModuleManager::terminate()
{
    for (Module* module : modules) {
        module->onDisable();
    }

    modules.clear();
}

void ModuleManager::SaveModulesConfig() {
    for (Module* module : modules) {
        module->SaveSettings();
    }
}

bool ModuleManager::doesAnyModuleHave(std::string settingName) {

    bool result = false;

    for (Module* mod : modules) {

        if(mod->settings.getSettingByName<bool>(settingName) != nullptr)
            if(mod->settings.getSettingByName<bool>(settingName)->value && mod->settings.getSettingByName<bool>("enabled")->value){
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
