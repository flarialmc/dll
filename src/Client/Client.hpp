#pragma once

#include "Hook/Manager.hpp"
#include "Module/Manager.hpp"
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
    inline static std::string version;
    inline static HMODULE currentModule = nullptr;

    inline static std::string path = Utils::getConfigsPath() + "\\main.flarial";
    static void SaveSettings() {

        try {
            std::ofstream outputFile(path);
            if (outputFile) {
                outputFile << settings.ToJson();
            } else {
                Logger::error("Failed to open settings file: {}", path);
            }
        } catch (const std::exception& e) {
            Logger::error("An error occurred while trying to save settings to {}: {}", path, e.what());
        }
    }

    static void LoadSettings() {
        std::ifstream inputFile(path);

        if (!inputFile) {
            Logger::error("Failed to open settings file: {}", path);
            return;
        }

        std::stringstream ss;
        ss << inputFile.rdbuf();
        settings.FromJson(ss.str());
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
