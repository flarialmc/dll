#pragma once

#include "Manager.hpp"
#include "../Events/EventHandler.hpp"
#include "Modules/Input/GUIKeyListener.hpp"
#include "Modules/Misc/SaveConfig/SaveConfigListener.hpp"
#include "Modules/Misc/RGB/rgbListener.hpp"
#include "Modules/Misc/TextAlias/TextAliasListener.hpp"
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
#include "Modules/EntityCounter/EntityCounter.hpp"
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
#include "Modules/Misc/CentreCursor/CentreCursorListener.hpp"
#include "Modules/Misc/Uninject/UninjectListener.hpp"
#include "Modules/CPSLimiter/CPSLimiter.hpp"
#include "Modules/BlockBreakIndicator/BlockBreakIndicator.hpp"
//#include "Modules/CompactChat/CompactChat.hpp"
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
//#include "Modules/Overlay/OverlayModule.hpp"
#include "Modules/AutoRQ/AutoRQ.hpp"
#include "Modules/HitPing/HitPing.hpp"
#include "Modules/InstantHurtAnimation/InstantHurtAnimation.hpp"
//#include "Modules/MovableChat/MovableChat.hpp"
#include <algorithm>

#include "Modules/ItemPhysics/ItemPhysics.hpp"
#include "Modules/OpponentReach/OpponentReach.hpp"
#include "Modules/ViewModel/ViewModel.hpp"
#include "Modules/PotionHUD/PotionHUD.hpp"
#include "Modules/FasterInventory/FasterInventory.hpp"

namespace ModuleManager {
    std::unordered_map<size_t, Module*> moduleMap;
    bool initialized = false;
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
    addModule(new MotionBlur());

    // Screen effects
    addModule(new Deepfry());
    addModule(new HueChanger());
    addModule(new PatarHD());
    addModule(new DVD());

    // FOV Changers
    addModule(new FOVChanger()); //1
    addModule(new Zoom()); //2
    addModule(new UpsideDown()); //3

    addModule(new ClickGUI());

    addModule(new FPSCounter());
    addModule(new CPSCounter());
    addModule(new IPDisplay());
    addModule(new ReachCounter());
    addModule(new ComboCounter());
    addModule(new PingCounter());
    addModule(new PotCounter());
    addModule(new ArrowCounter());
    addModule(new EntityCounter());
    addModule(new Time());
    addModule(new MEM());
    addModule(new Fullbright());
    addModule(new ForceCoords());
    addModule(new Keystrokes());
    addModule(new Sneak());
    addModule(new Sprint());
    addModule(new Hitbox());
    addModule(new ThirdPerson());
    addModule(new SnapLook());
    addModule(new HurtColor());
    addModule(new FogColor());
    addModule(new ArmorHUD());
    addModule(new TimeChanger());
    addModule(new RenderOptions());
    addModule(new PaperDoll());
    addModule(new GuiScale());
    addModule(new WeatherChanger());
    addModule(new TabList());
    addModule(new AutoGG());
    addModule(new TextHotkey());
    addModule(new NickModule());
    addModule(new FreeLook());
    addModule(new SpeedDisplay());
    addModule(new CPSLimiter());
    addModule(new BlockBreakIndicator());
    addModule(new Animations());
    addModule(new BlockOutline());
    addModule(new CommandHotkey());
    addModule(new NoHurtCam());
    addModule(new InventoryHUD());
    //addModule(new OverlayModule());
    addModule(new AutoRQ());
    addModule(new HitPing());
    addModule(new InstantHurtAnimation());
    addModule(new OpponentReach());
    addModule(new ViewModel());
    addModule(new PotionHUD());
    addModule(new FasterInventory());

    //addModule(new MovableChat());
    //addModule(new CompactChat());
    addModule(new ItemPhysics());


    EventHandler::registerListener(new GUIKeyListener("GuiKeyListener"));
    EventHandler::registerListener(new DiscordRPCListener("DiscordRPC"));
    EventHandler::registerListener(new UninjectListener("Uninject"));
    EventHandler::registerListener(new SaveConfigListener("SaveConfig"));
    EventHandler::registerListener(new CentreCursorListener("CentreCursor"));
    EventHandler::registerListener(new rgbListener("RGB Controller"));
    EventHandler::registerListener(new TextAliasListener("TextAlias"));

    initialized = true;
}

void ModuleManager::terminate() {
    initialized = false;
    for (const auto& pair : moduleMap) {
        if (pair.second != nullptr)
            pair.second->terminate();
    }
    moduleMap.clear();
}

void ModuleManager::SaveModulesConfig() {
    for (const auto& pair : moduleMap) {
        pair.second->saveSettings();
    }
}
// TODO: use enums?
bool ModuleManager::doesAnyModuleHave(const std::string& settingName) {

    bool result = false;

    if(!ModuleManager::initialized) return false;

    for (const auto& pair : moduleMap) {
        if(!pair.second) continue;

        if (pair.second->settings.getSettingByName<bool>(settingName) != nullptr)
            if (pair.second->settings.getSettingByName<bool>(settingName)->value &&
                pair.second->isEnabled() && (pair.second->active || pair.second->defaultKeybind.empty())) {
                result = true;
                break;
            }
    }

    return result;

}

Module* ModuleManager::getModule(const std::string& name) {
    size_t hash = std::hash<std::string>{}(name);

    return moduleMap[hash];
}