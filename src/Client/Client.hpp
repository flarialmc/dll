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
private:
    inline static std::string privatePath = Utils::getConfigsPath() + "\\PRIVATE";
public:
    static std::string current_commit;
    static float elapsed;
    static uint64_t start;

    static std::vector<std::string> availableConfigs;

    static std::vector<std::string> getPlayersVector(const nlohmann::json &data);

    static void UnregisterActivationHandler();

    static void createConfig(std::string name);

    static void deleteConfig(std::string name);

    static void switchConfig(std::string name, bool reload = true);

    static void loadAvailableConfigs();

    static void initialize();

    static bool disable;

    static void centerCursor();

    static Settings settings;
    inline static nlohmann::json globalSettings;
    inline static std::string version;
    inline static HMODULE currentModule = nullptr;

    inline static std::string path = Utils::getConfigsPath() + "\\default.json";

    static void SaveSettings() {
        path = Utils::getConfigsPath() + "\\" + settings.getSettingByName<std::string>("currentConfig")->value;
        try {
            //clear files
            std::ofstream cls(path, std::ofstream::out | std::ofstream::trunc); cls.close();
            std::ofstream pCls(privatePath, std::ofstream::out | std::ofstream::trunc); pCls.close();

            //open for writing
            std::ofstream cFile(path, std::ios::app);
            std::ofstream pFile(privatePath, std::ios::app);

            //write configs
            pFile << settings.ToJson();
            pFile.close();
            cFile << "{";

            auto it = ModuleManager::moduleMap.begin();
            while (it != ModuleManager::moduleMap.end()) {
                cFile << "\n  \"" << it->second->name << "\": " << it->second->settings.ToJson();
                ++it;
                if (it != ModuleManager::moduleMap.end()) { cFile << ","; }
            }
            cFile << "\n}";
            cFile.close();
        } catch (const std::exception& e) {
            Logger::error("An error occurred while trying to save settings: {}", e.what());
        }

        ScriptManager::saveSettings();
    }

    static void LoadSettings() {
        // load private client settings

        std::ifstream privateFile(privatePath);
        if (!privateFile) return;

        std::stringstream pSS;
        pSS << privateFile.rdbuf();
        std::string pStr = pSS.str();

        if (pStr.empty()) return;

        try { settings.FromJson(pStr); }
        catch (const nlohmann::json::parse_error& e) {
            Logger::error("Failed to parse JSON: {}", e.what());
        }

        path = Utils::getConfigsPath() + "\\" + Client::settings.getSettingByName<std::string>("currentConfig")->value;

        std::ifstream inputFile(path);
        if (!inputFile) return;

        std::stringstream ss;
        ss << inputFile.rdbuf();
        std::string str = ss.str();

        if (str.empty()) {
            Logger::error("Settings String is empty");
            return;
        }

        try { globalSettings = nlohmann::json::parse(str); } 
        catch (const nlohmann::json::parse_error& e) {
            Logger::error("Failed to parse JSON: {}", e.what());
        }

    }

    static void CheckSettingsFile() {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(std::filesystem::path(path).parent_path());

            std::ofstream file(path, std::ios::app);
            std::ofstream pFile(privatePath, std::ios::app);
            //SetFileAttributes(privatePath, FILE_ATTRIBUTE_HIDDEN);    idk, might actually do this in the future
            if (!file || !pFile) {
                Logger::error("Failed to create settings file: {}", path);
            }
        }
    }
};
