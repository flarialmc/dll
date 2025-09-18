#include "Manager.hpp"


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
	registerAllModules();
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
