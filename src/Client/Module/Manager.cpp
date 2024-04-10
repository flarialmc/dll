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
#include "Modules/FOVChanger/FOVChanger.hpp"
#include "Modules/UpsideDown/UpsideDown.hpp"
#include "Modules/Animations/Animations.hpp"
#include "Modules/DVD Screen/dvd.hpp"
#include "Modules/BlockOutline/BlockOutline.hpp"
#include "Modules/Hitbox/Hitbox.hpp"
#include "Modules/InventoryHUD//InventoryHUD.hpp"
#include "Modules/NoHurtCam/NoHurtCam.hpp"
#include "Modules/CommandHotkey/CommandHotkey.hpp"
#include "Modules/Misc/DiscordRPC/DiscordRPCListener.hpp"
#include "Modules/MovableChat/MovableChat.hpp"
#include <algorithm>

namespace ModuleManager {
    std::unordered_map<size_t, Module*> moduleMap;
    std::vector<std::string> onlineUsers;
}

void ModuleManager::addModule(Module* module) {
    size_t hash = std::hash<std::string>{}(module->name);
    moduleMap[hash] = module;
}

std::vector<Module*> ModuleManager::getModules() {
    std::vector<Module*> modulesVector;
    for (const auto& pair : moduleMap) {
        modulesVector.push_back(pair.second);
    }
    return modulesVector;
}

void ModuleManager::initialize() {
    ModuleManager::addModule(new MotionBlur());

    // Screen effects
    ModuleManager::addModule(new Deepfry());
    ModuleManager::addModule(new HueChanger());
    ModuleManager::addModule(new PatarHD());
    ModuleManager::addModule(new DVD());

    // FOV Changers
    ModuleManager::addModule(new FOVChanger()); //1
    ModuleManager::addModule(new Zoom()); //2
    ModuleManager::addModule(new UpsideDown()); //3

    ModuleManager::addModule(new ClickGUI());

    ModuleManager::addModule(new FPSCounter());
    ModuleManager::addModule(new CPSCounter());
    ModuleManager::addModule(new IPDisplay());
    ModuleManager::addModule(new ReachCounter());
    ModuleManager::addModule(new ComboCounter());
    ModuleManager::addModule(new PingCounter());
    ModuleManager::addModule(new PotCounter());
    ModuleManager::addModule(new ArrowCounter());
    ModuleManager::addModule(new Time());
    ModuleManager::addModule(new MEM());
    ModuleManager::addModule(new Fullbright());
    ModuleManager::addModule(new ForceCoords());
    ModuleManager::addModule(new Keystrokes());
    ModuleManager::addModule(new Sneak());
    ModuleManager::addModule(new Sprint());
    ModuleManager::addModule(new Hitbox());
    ModuleManager::addModule(new ThirdPerson());
    ModuleManager::addModule(new SnapLook());
    ModuleManager::addModule(new HurtColor());
    ModuleManager::addModule(new FogColor());
    ModuleManager::addModule(new ArmorHUD());
    ModuleManager::addModule(new TimeChanger());
    ModuleManager::addModule(new RenderOptions());
    ModuleManager::addModule(new PaperDoll());
    ModuleManager::addModule(new GuiScale());
    ModuleManager::addModule(new WeatherChanger());
    ModuleManager::addModule(new TabList());
    ModuleManager::addModule(new AutoGG());
    ModuleManager::addModule(new TextHotkey());
    ModuleManager::addModule(new NickModule());
    ModuleManager::addModule(new FreeLook());
    ModuleManager::addModule(new SpeedDisplay());
    ModuleManager::addModule(new CPSLimiter());
    ModuleManager::addModule(new BlockBreakIndicator());
    ModuleManager::addModule(new Animations());
    ModuleManager::addModule(new BlockOutline());
    ModuleManager::addModule(new CommandHotkey());
    ModuleManager::addModule(new NoHurtCam());
    ModuleManager::addModule(new InventoryHUD());
    //ModuleManager::addModule(new MovableChat());
    //ModuleManager::addModule(new CompactChat());

    EventHandler::registerListener(new GUIKeyListener("GuiKeyListener"));
    EventHandler::registerListener(new DiscordRPCListener("DiscordRPC"));
    EventHandler::registerListener(new UninjectListener("Uninject"));
    EventHandler::registerListener(new SaveConfigListener("SaveConfig"));
    EventHandler::registerListener(new CentreCursorListener("CentreCursor"));
    EventHandler::registerListener(new rgbListener("RGB Controller"));
    EventHandler::registerListener(new TextAliasListener("TextAlias"));
}

void ModuleManager::terminate() {
    for (const auto& pair : ModuleManager::moduleMap) {
        pair.second->terminate();
    }
    moduleMap.clear();
}

void ModuleManager::SaveModulesConfig() {
    for (const auto& pair : ModuleManager::moduleMap) {
        pair.second->saveSettings();
    }
}
// TODO: use enums?
bool ModuleManager::doesAnyModuleHave(const std::string& settingName) {

    bool result = false;

    for (const auto& pair : ModuleManager::moduleMap) {

        if (pair.second->settings.getSettingByName<bool>(settingName) != nullptr)
            if (pair.second->settings.getSettingByName<bool>(settingName)->value &&
                pair.second->isEnabled()) {
                result = true;
                break;
            }
    }

    return result;

}

Module* ModuleManager::getModule(const std::string& name) {
    size_t hash = std::hash<std::string>{}(name);

    auto it = moduleMap.find(hash);
    if (it != moduleMap.end()) {
        return it->second;
    }

    return nullptr;
}
