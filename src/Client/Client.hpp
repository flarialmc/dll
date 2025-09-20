#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <lib/json/json_fwd.hpp>
#include <windows.h>

// Forward declarations
class Settings;

#define ADD_SETTING(setting, value) \
if (Client::settings.getSettingByName<decltype(value)>(setting) == nullptr) \
Client::settings.addSetting(setting, value);

namespace fs = std::filesystem;

class Client {
	static std::string privatePath;
public:
	static std::string current_commit;
	static float elapsed;
	static uint64_t start;
	static std::vector<std::string> availableConfigs;
	static std::vector<std::string> getPlayersVector(const nlohmann::json& data);
	static bool init;

	static void UnregisterActivationHandler();

	static void createConfig(std::string name);
	static void switchConfig(std::string name, bool deleting = false);
	static void deleteConfig(std::string name);

	static void loadAvailableConfigs();

	static void PerformPostLegacySetup();

	static void initialize();

	static bool disable;

	static void centerCursor();

	static std::string activeConfig;
	static bool hasLegacySettings;
	static bool softLoadLegacy;
	static bool privateInit;
	static bool savingSettings;
	static bool savingPrivate;

	static Settings settings;
	static Settings legacySettings;

	static nlohmann::json globalSettings;
	static std::string version;
	static HMODULE currentModule; // Keep as HMODULE for Windows API compatibility

	static std::string path;
	static std::string legacyPath;
	static std::string legacyDir;

	static void LoadLegacySettings();
	static void SavePrivate();
	static void LoadPrivate();
	static void SaveSettings();
	static void LoadSettings();
	static void CheckSettingsFile();
};
