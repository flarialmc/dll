#include "Manager.hpp"

#include "Modules/Misc/Input/GUIKeyListener.hpp"
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
#include "Modules/Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "Modules/HitPing/HitPing.hpp"
#include "Modules/InstantHurtAnimation/InstantHurtAnimation.hpp"
//#include "Modules/MovableChat/MovableChat.hpp"
#include <algorithm>
#include <Modules/Misc/InputImGUi/GUIMouseListener.hpp>
#include <Modules/Misc/InputImGUi/GUIKeyListener.hpp>
#include <Modules/Misc/PackChanger/PackChanger.hpp>

#include "Modules/ItemPhysics/ItemPhysics.hpp"
#include "Modules/Crosshair/Crosshair.hpp"
#include "Modules/CustomCrosshair/CustomCrosshair.hpp"
#include "Modules/HiveStat/HiveStat.hpp"
#include "Modules/OpponentReach/OpponentReach.hpp"
#include "Modules/ViewModel/ViewModel.hpp"
#include "Modules/PotionHUD/PotionHUD.hpp"
#include "Modules/FasterInventory/FasterInventory.hpp"
#include "Modules/Waypoints/Waypoints.hpp"
#include "Modules/JavaInventoryHotkeys/JavaInventoryHotkeys.hpp"

#include "Modules/EntityCounter/EntityCounter.hpp"
#include "Modules/MovableHUD/MovableHUD.hpp"
#include "Modules/MovableScoreboard/MovableScoreboard.hpp"
#include "Modules/MovableTitle/MovableTitle.hpp"
#include "Modules/MovableBossbar/MovableBossbar.hpp"
#include "Modules/MovableChat/MovableChat.hpp"
#include "Modules/MovableCoordinates/MovableCoordinates.hpp"
#include "Modules/MovableHotbar/MovableHotbar.hpp"
#include "Modules/NullMovement/NullMovement.hpp"
#include "../../Scripting/Scripting.hpp"
#include "../../Scripting/EventManager/ScriptingEventManager.hpp"
#include "Modules/Cursor/Cursor.hpp"
#include "Modules/RawInputBuffer/RawInputBuffer.hpp"
#include "Modules/JavaDynamicFOV/JavaDynamicFOV.hpp"
#include "Modules/ItemUseDelayFix/ItemUseDelayFix.hpp"
#include "../../Scripting/Console/ConsoleService.hpp"

namespace ModuleManager {
    std::map<size_t, std::shared_ptr<Module>> moduleMap;
    std::vector<std::shared_ptr<Listener>> services;
    bool initialized = false;
    bool restartModules = false;
}

std::vector<std::shared_ptr<Module>> ModuleManager::getModules() { // TODO: some module is null here for some reason, investigation required
    std::vector<std::shared_ptr<Module>> modulesVector;
    for (const auto& pair : moduleMap) {
        if(pair.second == nullptr) continue;
        modulesVector.push_back(pair.second);
    }
    return modulesVector;
}

void ModuleManager::initialize() {
    addModule<MotionBlur>();

    // Screen effects
    addModule<Deepfry>();
    addModule<HueChanger>();
    addModule<PatarHD>();
    addModule<DVD>();

    // FOV Changers
    addModule<FOVChanger>(); //1
    addModule<Zoom>(); //2
    addModule<UpsideDown>(); //3

    addModule<ClickGUI>();

    addModule<FPSCounter>();
    addModule<CPSCounter>();
    addModule<IPDisplay>();
    addModule<ReachCounter>();
    addModule<ComboCounter>();
    addModule<PingCounter>();
    addModule<PotCounter>();
    addModule<ArrowCounter>();
    addModule<EntityCounter>();
    addModule<Time>();
    addModule<MEM>();
    addModule<Fullbright>();
    addModule<ForceCoords>();
    addModule<Keystrokes>();
    addModule<Sneak>();
    addModule<Sprint>();
    addModule<Hitbox>();
    addModule<HurtColor>();
    addModule<ThirdPerson>();
    addModule<JavaDynamicFOV>();

    addModule<SnapLook>();
    addModule<FogColor>();
    addModule<ArmorHUD>();
    addModule<TimeChanger>();
    addModule<RenderOptions>();
    addModule<PaperDoll>();
    addModule<GuiScale>();
    addModule<TabList>();
    addModule<WeatherChanger>();
    addModule<NickModule>();
    addModule<FreeLook>();

    addModule<AutoGG>();
    addModule<TextHotkey>();
    addModule<SpeedDisplay>();
    addModule<CPSLimiter>();
    addModule<BlockBreakIndicator>();
    addModule<Animations>();

    addModule<BlockOutline>();
    addModule<CommandHotkey>();
    addModule<NoHurtCam>();
    addModule<InventoryHUD>();
    addModule<AutoRQ>();
    addModule<HitPing>();
    addModule<InstantHurtAnimation>();
    addModule<OpponentReach>();
    addModule<ViewModel>();
    addModule<PotionHUD>();
    addModule<FasterInventory>();
    //addModule<MovableHUD>();
    addModule<MovableScoreboard>();
    addModule<MovableTitle>();
    addModule<MovableBossbar>();
    addModule<MovableChat>();
    addModule<MovableCoordinates>();
    addModule<MovableHotbar>();
    // addModule<CompactChat>();
    addModule<ItemPhysics>();

    if (VersionUtils::checkAboveOrEqual(21, 60)) {
        addModule<JavaInventoryHotkeys>();
    }

    addModule<HiveStat>();
    addModule<Waypoints>();

    addModule<NullMovement>();
    addModule<CustomCrosshair>();

    if (!VersionUtils::checkAboveOrEqual(21, 50)) {
        addModule<Cursor>();
    }


    addModule<RawInputBuffer>();
    //addModule<ItemUseDelayFix>();

    addService<GUIKeyListener>();
    if (!VersionUtils::checkAboveOrEqual(21, 60)) {
        addService<PackChanger>();
    }
    addService<DiscordRPCListener>();
    addService<UninjectListener>();
    addService<SaveConfigListener>();
    addService<CentreCursorListener>();
    addService<rgbListener>();
    addService<HiveModeCatcherListener>();
    addService<ImGUIMouseListener>();
    addService<ImGUIKeyListener>();

    addService<ConsoleService>();
    Scripting::loadModules();

    initialized = true;
    Scripting::instalized = true;
}

void ModuleManager::terminate() {
    initialized = false;
    for (const auto& pair : moduleMap) {
        if (pair.second != nullptr)
            pair.second->terminate();
    }
    moduleMap.clear();
    services.clear();
}




void restart(){
    Scripting::instalized = false;
    Scripting::unloadModules();
    Scripting::loadModules();
    Scripting::instalized = true;
}


void ModuleManager::syncState() {
    if(!ModuleManager::initialized) return;
    for (const auto& [key, module] : moduleMap) {
        if (!module || module->enabledState == module->isEnabled() || module->delayDisable) {
            continue;
        }

        if (module->enabledState) {
            module->onEnable();
        } else {
            module->onDisable();
        }
    }
    if (ModuleManager::restartModules) {
        ModuleManager::restartModules = false;
        restart();
    }
}


void ModuleManager::SaveModulesConfig() {
    for (const auto& module : getModules()) {
        module->saveSettings();
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

std::shared_ptr<Module> ModuleManager::getModule(const std::string& name) {
    size_t hash = std::hash<std::string>{}(name);

    return moduleMap[hash];
}
