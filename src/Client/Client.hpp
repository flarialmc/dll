#pragma once

#include "Hook/Manager.hpp"
#include "Module/Manager.hpp"
#include <vector>
#include "Scripting/ScriptManager.hpp"
#include "Utils/APIUtils.hpp"

#define ADD_SETTING(setting, value) \
if (Client::settings.getSettingByName<decltype(value)>(setting) == nullptr) \
Client::settings.addSetting(setting, value);

namespace fs = std::filesystem;

class Client {
private:
	inline static std::string privatePath = Utils::getConfigsPath() + "\\PRIVATE";
public:
	static std::string current_commit;
	static float elapsed;
	static uint64_t start;
	static std::vector<std::string> availableConfigs;
	static std::vector<std::string> getPlayersVector(const nlohmann::json& data);

	static void UnregisterActivationHandler();

	static void createConfig(std::string name);

	static void deleteConfig(std::string name);

	static void loadAvailableConfigs();

	static void initialize();

	static bool disable;

	static void centerCursor();

	inline static std::string activeConfig;

	static Settings settings;
	static inline Settings legacySettings;

	inline static nlohmann::json globalSettings;
	inline static std::string version;
	inline static HMODULE currentModule = nullptr;

	inline static std::string path = Utils::getConfigsPath() + "\\default.json";
	inline static std::string legacyPath = Utils::getConfigsPath() + "\\main.flarial";
	inline static std::string legacyDir = Utils::getConfigsPath() + "\\Legacy";

	static void LoadLegacySettings() {
		std::ifstream inputFile(legacyPath);

		if (!inputFile) {
			Logger::info("No legacy settings found");
			return;
		}

		inputFile.close();

		std::error_code ec_rename;
		fs::rename(Utils::getConfigsPath(), Utils::getClientPath() + "\\Legacy", ec_rename);

		if (ec_rename) {
			LOG_ERROR("Failed to rename Config directory to Legacy: {}", ec_rename.message());
			return;
		}
		else Logger::success("Renamed Config directory to Legacy");

		if (fs::create_directory(Utils::getConfigsPath())) LOG_ERROR("Config directory successfully created");
		else {
			LOG_ERROR("Failed to create Config directory");
			return;
		}

		fs::rename(Utils::getClientPath() + "\\Legacy", legacyDir, ec_rename);
		if (ec_rename) {
			LOG_ERROR("Failed to move Legacy folder to Config: {}", ec_rename.message());
			return;
		}
		else Logger::success("Moved Legacy dir to Config dir");


		legacyPath = legacyDir + "\\main.flarial";

		std::ifstream legacyFile(legacyPath);

		if (!legacyFile) {
			Logger::info("No legacy settings found");
			return;
		}

		std::stringstream ss;
		ss << legacyFile.rdbuf();
		legacyFile.close();
		legacySettings.FromJson(ss.str());
	}

	static void SavePrivate() {
		// save PRIVATE
		try {
			if (!settings.getSettingByName<std::string>("currentConfig")) return Logger::warn("No Client Settings to save to PRIVATE");

			// for some reason u cant directly clear file content when its hidden??? ok then
			DWORD originalAttributes = INVALID_FILE_ATTRIBUTES;
			originalAttributes = GetFileAttributesA(privatePath.c_str());
			if (originalAttributes == INVALID_FILE_ATTRIBUTES) return LOG_ERROR("Failed to get attributes of PRIVATE: {}", GetLastError());
			DWORD attributesToClear = 0;
			if (originalAttributes & FILE_ATTRIBUTE_HIDDEN) attributesToClear |= FILE_ATTRIBUTE_HIDDEN;
			if (attributesToClear != 0)
				if (!SetFileAttributesA(privatePath.c_str(), originalAttributes & ~attributesToClear))
					return LOG_ERROR("Failed to remove temporary attributes from PRIVATE: {}", GetLastError());

			std::ofstream pCls(privatePath, std::ofstream::out | std::ofstream::trunc);
			if (!pCls.is_open()) return LOG_ERROR("Could not clear the contents of PRIVATE: {}", GetLastError());
			pCls.close();

			if (!SetFileAttributesA(privatePath.c_str(), originalAttributes)) LOG_ERROR("Failed to set PRIVATE as a hidden file {}", GetLastError());

			std::ofstream pFile(privatePath, std::ios::app);

			pFile << settings.ToJson();
			pFile.close();
		}
		catch (const std::exception& e) {
			LOG_ERROR("An error occurred while trying to save settings: {}", e.what());
		}
	}

	static void LoadPrivate() {
		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading PRIVATE");

		std::ifstream privateFile(privatePath);
		if (!privateFile) return;

		std::stringstream pSS;

		pSS << privateFile.rdbuf();
		privateFile.close();

		std::string pStr = pSS.str();

		if (pStr.empty() || pStr == "null") return Logger::warn("Empty or null PRIVATE");

		try { settings.FromJson(pStr); }
		catch (const nlohmann::json::parse_error& e) {
			LOG_ERROR("Failed to parse JSON: {}", e.what());
		}

		path = Utils::getConfigsPath() + "\\" + settings.getSettingByName<std::string>("currentConfig")->value;

		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded PRIVATE");
	}

	static void SaveSettings() {
		if (path.empty()) path = Utils::getConfigsPath() + "\\" + settings.getSettingByName<std::string>("currentConfig")->value;

		try {
			std::ofstream cls(path, std::ofstream::out | std::ofstream::trunc); cls.close();
			std::ofstream cFile(path, std::ios::app);

			cFile << "{";

			auto it = ModuleManager::moduleMap.begin();
			while (it != ModuleManager::moduleMap.end()) {
				if (it->second == nullptr) {
					++it;
					continue;
				}
				cFile << "\n  \"" << it->second->name << "\": " << it->second->settings.ToJson();
				++it;
				if (it != ModuleManager::moduleMap.end()) cFile << ",";
			}
			cFile << "\n}";
			cFile.close();
		}
		catch (const std::exception& e) {
			Logger::error("An error occurred while trying to save settings: {}", e.what());
		}

		ScriptManager::saveSettings();
	}

	static void LoadSettings() {
		if (!settings.getSettingByName<std::string>("currentConfig")) return LOG_ERROR("No PRIVATE settings found");
		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);

		if (path.empty()) path = Utils::getConfigsPath() + "\\" + Client::settings.getSettingByName<std::string>("currentConfig")->value;

		std::ifstream inputFile(path);
		if (!inputFile) return;

		std::stringstream ss;
		ss << inputFile.rdbuf();
		inputFile.close();
		std::string str = ss.str();

		if (str.empty()) {
			Logger::warn("Settings String is empty");
			globalSettings.clear();
			return;
		}

		try { globalSettings = nlohmann::json::parse(str); }
		catch (const nlohmann::json::parse_error& e) {
			LOG_ERROR("Failed to parse JSON: {}", e.what());
		}

		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);
	}

	static void CheckSettingsFile() {
		if (!fs::exists(path)) {
			fs::create_directories(fs::path(path).parent_path());

			std::ofstream file(path, std::ios::app);
			std::ofstream pFile(privatePath, std::ios::app);

			DWORD attributes = GetFileAttributesA(privatePath.c_str());

			if (SetFileAttributesA(privatePath.c_str(), attributes | FILE_ATTRIBUTE_HIDDEN)) Logger::success("PRIVATE is now a hidden file");
			else LOG_ERROR("Failed to set PRIVATE as a hidden file");
			if (!file || !pFile) LOG_ERROR("Failed to create settings file: {}", path);

			file.close();
			pFile.close();
		}
	}
};