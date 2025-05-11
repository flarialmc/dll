#pragma once

#include "Hook/Manager.hpp"
#include "Module/Manager.hpp"
#include "Scripting/ScriptManager.hpp"
#include <vector>

#include "Utils/APIUtils.hpp"

#define ADD_SETTING(setting, value) \
if (Client::settings.getSettingByName<decltype(value)>(setting) == nullptr) \
Client::settings.addSetting(setting, value);

class Client {
public:
    static std::string current_commit;
    static float elapsed;
    static uint64_t start;

    static std::vector<std::string> availableConfigs;

    static std::vector<std::string> getPlayersVector(const nlohmann::json &data);

    static void UnregisterActivationHandler();

    static void createConfig(std::string name);

    static void deleteConfig(std::string name);

    static void loadAvailableConfigs();

    static void initialize();

    static bool disable;

    static void centerCursor();

    static Settings settings;
    inline static nlohmann::json globalSettings;
    inline static std::string version;
    inline static HMODULE currentModule = nullptr;

    inline static std::string path = Utils::getConfigsPath() + "\\main.json";

    static void SaveSettings() {
        try {
            std::ofstream cls(path, std::ofstream::out | std::ofstream::trunc); cls.close(); //clear

            std::ofstream cFile(path, std::ios::app);
            cFile << "{ \n";
            for (const auto& pair : ModuleManager::moduleMap) {
                cFile << '"' << pair.second->name << "\": " << pair.second->settings.ToJson() << ", \n";
            }
            cFile << "\"main\":" << settings.ToJson() << "\n }";

        } catch (const std::exception& e) {
            Logger::error("An error occurred while trying to save settings: {}", e.what());
        }

        ScriptManager::saveSettings();
    }

    static void LoadSettings() {
        std::ifstream inputFile(path);
        if (!inputFile) return;

        std::stringstream ss;
        ss << inputFile.rdbuf();
        inputFile.close();
        std::string str = ss.str();

        if (str.empty()) {
            Logger::error("Settings String is empty");
            return;
        }

        try { globalSettings = nlohmann::json::parse(str); } 
        catch (const nlohmann::json::parse_error& e) {
            Logger::error("Failed to parse JSON: {}", e.what());
        }
        try {
            settings.FromJson(globalSettings["main"].dump());
        }
        catch (const std::exception& e) { Logger::error(e.what()); }
        
    }

    static void CheckSettingsFile() {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(std::filesystem::path(path).parent_path());

            std::ofstream file(path, std::ios::app);
            if (!file) {
                Logger::error("Failed to create settings file: {}", path);
            }
        }
    }
};
