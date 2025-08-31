#include "Manager.hpp"

#include "Modules/Misc/Input/GUIKeyListener.hpp"
#include "Modules/Misc/SaveConfig/SaveConfigListener.hpp"
#include "Modules/Misc/RGB/rgbListener.hpp"
#include "Modules/Misc/TextAlias/TextAliasListener.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Module.hpp"
#include "Modules/FPS/FPS.hpp"
#include "Modules/Waila/Waila.hpp"
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
#include "Modules/AutoRQ/AutoRQ.hpp"
#include "Modules/Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "Modules/HitPing/HitPing.hpp"
#include "Modules/InstantHurtAnimation/InstantHurtAnimation.hpp"
#include <Modules/Misc/InputImGUi/GUIMouseListener.hpp>
#include <Modules/Misc/InputImGUi/GUIKeyListener.hpp>
#include <Modules/Misc/PackChanger/PackChanger.hpp>
#include <Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp>

#include "Modules/202020/202020.hpp"
#include "Modules/ItemPhysics/ItemPhysics.hpp"
#include "Modules/CustomCrosshair/CustomCrosshair.hpp"
#include "Modules/HiveStat/HiveStat.hpp"
#include "Modules/OpponentReach/OpponentReach.hpp"
#include "Modules/ViewModel/ViewModel.hpp"
#include "Modules/PotionHUD/PotionHUD.hpp"
#include "Modules/FasterInventory/FasterInventory.hpp"
#include "Modules/Waypoints/Waypoints.hpp"
#include "Modules/JavaInventoryHotkeys/JavaInventoryHotkeys.hpp"
#include "Modules/MovableScoreboard/MovableScoreboard.hpp"
#include "Modules/MovableTitle/MovableTitle.hpp"
#include "Modules/MovableBossbar/MovableBossbar.hpp"
#include "Modules/MovableChat/MovableChat.hpp"
#include "Modules/MovableCoordinates/MovableCoordinates.hpp"
#include "Modules/MovableHotbar/MovableHotbar.hpp"
#include "Modules/NullMovement/NullMovement.hpp"
#include "Modules/ModernKeybindHandling/ModernKeybindHandling.hpp"

#include "Modules/RawInputBuffer/RawInputBuffer.hpp"
#include "Modules/JavaDynamicFOV/JavaDynamicFOV.hpp"
#include "Modules/ItemUseDelayFix/ItemUseDelayFix.hpp"

#include "Modules/Mousestrokes/Mousestrokes.hpp"
#include "Scripting/ScriptManager.hpp"
#include "Modules/AutoPerspective/AutoPerspective.hpp"
#include "Modules/BlockHit/BlockHit.hpp"
#include "Modules/CinematicCamera/CinematicCamera.hpp"
#include "Modules/LowHealthIndicator/LowHealthIndicator.hpp"
#include "Modules/PlayerNotifier/PlayerNotifier.hpp"
#include "Modules/ZeqaUtils/ZeqaUtils.hpp"
#include "Modules/MumbleLink/MumbleLink.hpp"
#include "Modules/MaterialBinLoader/MaterialBinLoader.hpp"

#include "Modules/MinimalViewBobbing/MinimalViewBobbing.hpp"
#include "Modules/Lewis/Lewis.hpp"
#include "Modules/Coordinates/Coordinates.hpp"
#include "Modules/DisableMouseWheel/DisableMouseWheel.hpp"
#include "Modules/DebugMenu/DebugMenu.hpp"
#include "Modules/DirectionHUD/DirectionHUD.hpp"
#include "Modules/JavaViewBobbing/JavaViewBobbing.hpp"
#include "Modules/DeathLogger/DeathLogger.hpp"
#include "Modules/HurtColor/HurtColor.hpp"
#include "Modules/Twerk/Twerk.hpp"
#include "Modules/MovableDayCounter/MovableDayCounter.hpp"
#include "Modules/SkinStealer/SkinStealer.hpp"
#include "Modules/GlintColor/GlintColor.hpp"
#include "Modules/ChunkBorder/ChunkBorder.hpp"
#include "Modules/CompactChat/CompactChat.hpp"
#include "Modules/MessageLogger/MessageLogger.hpp"
#include "Modules/TotemCounter/TotemCounter.hpp"
#include "Modules/BetterHungerBar/BetterHungerBar.hpp"

#ifdef COMPILE_DOOM
	#include "Modules/Doom/Doom.hpp"
#endif

void ModuleManager::getModules() {
	for (const auto& pair : moduleMap) {
		if (pair.second == nullptr) continue;
		modulesVector.push_back(pair.second);
	}
}

bool compareEnabled(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
	return obj1->isEnabled() >
		obj2->isEnabled();
}

bool compareFavorite(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
	return obj1->settings.getSettingByName<bool>("favorite")->value >
		obj2->settings.getSettingByName<bool>("favorite")->value;
}


bool compareNames(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
	return obj1->name < obj2->name;
}


void ModuleManager::updateModulesVector() {
	if (modulesVector.empty()) getModules();
	if (Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value)
		std::sort(modulesVector.begin(), modulesVector.end(), compareEnabled);
	else std::sort(modulesVector.begin(), modulesVector.end(), compareNames);
	std::sort(modulesVector.begin(), modulesVector.end(), compareFavorite);
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
	addModule<Module202020>();
	addModule<FPS>();
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
	addModule<Keystrokes>();
	addModule<Sneak>();
	addModule<Sprint>();
	addModule<Hitbox>();
	if (VersionUtils::checkAboveOrEqual(21, 80)) addModule<GlintColor>();
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

	addModule<AutoPerspective>();

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
	addModule<MovableDayCounter>();
	// addModule<CompactChat>();
	if(!VersionUtils::checkAboveOrEqual(21, 100)) {
		addModule<ItemPhysics>();
	}

	addModule<Mousestrokes>();

	if (VersionUtils::checkAboveOrEqual(21, 40)) {
		addModule<JavaInventoryHotkeys>();
		addModule<BlockHit>();
	}


	addModule<HiveStat>();
	addModule<Waypoints>();

	addModule<NullMovement>();
	addModule<ModernKeybindHandling>();
	addModule<CustomCrosshair>();
	addModule<Waila>();
	addModule<SkinStealer>();

	addModule<RawInputBuffer>();
	if (VersionUtils::checkAboveOrEqual(21, 00)) { // Due to entt
		addModule<LowHealthIndicator>();
	}
	addModule<PlayerNotifier>();
	//addModule<ItemUseDelayFix>();
	addModule<ZeqaUtils>();
	addModule<MumbleLink>();
	if(VersionUtils::checkAboveOrEqual(21, 50) && !VersionUtils::checkAboveOrEqual(21, 80)) {
	    addModule<MaterialBinLoader>();
	}

	addModule<MinimalViewBobbing>();

	addModule<Lewis>();
	addModule<Coordinates>();
	addModule<DisableMouseWheel>();
	addModule<JavaDebugMenu>();
	addModule<DirectionHUD>();

	addModule<JavaViewBobbing>();

	addModule<DeathLogger>();
	addModule<Twerk>();
	addModule<CinematicCamera>();
	addModule<ChunkBorder>();
	addModule<CompactChat>();
	addModule<MessageLogger>();
	addModule<TotemCounter>();
	addModule<BetterHungerBar>();

#ifdef COMPILE_DOOM
	addModule<DoomModule>();
#endif

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
	addService<ScriptMarketplace>();

	initialized = true;
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

void ModuleManager::restart() {
	initialized = false;
	Client::LoadSettings();
	for (const auto& pair : moduleMap) {
		if (pair.second) {
			std::shared_ptr mod = getModule(pair.second->name);
			if (mod != nullptr) {
				bool old = mod->enabledState;
				if (mod->isEnabled()) mod->onDisable();
				mod->settings.reset();
				mod->prevAlignments = std::vector<DWRITE_TEXT_ALIGNMENT>(100, DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
				mod->loadSettings();
			}
		}
	}
	
	initialized = true;
	Client::PerformPostLegacySetup();
	ScriptManager::reloadScripts();
	Client::SaveSettings();
}


void ModuleManager::syncState() {
	if (!initialized) return;

	ScriptManager::_reloadScripts();

	if (restartModules) {
		restartModules = false;
		restart();
		return;
	}
	for (const auto& [key, module] : moduleMap) {
		if (!module || module->enabledState == module->isEnabled() || module->delayDisable) continue;

		if (module->enabledState) module->onEnable();
		else module->onDisable();
	}
}

// TODO: use enums?
bool ModuleManager::doesAnyModuleHave(const std::string& settingName) {

	bool result = false;

	if (!ModuleManager::initialized) return false;

	for (const auto& pair : moduleMap) {
		if (!pair.second) continue;

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

	auto it = moduleMap.find(hash);

	if (it != moduleMap.end()) return moduleMap[hash];
	else return nullptr;
}
