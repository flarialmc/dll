#pragma once

#include "Hook/Manager.hpp"
#include "Module/Manager.hpp"
#include <vector>

class Client {
public:
    static std::vector<std::string> onlinePlayers;

    static std::string current_commit;

    static std::vector<std::string> getPlayersVector(const nlohmann::json &data);

    static void setWindowTitle(std::wstring title);

    static void initialize();

    static bool disable;

    static void centerCursor();

    static std::string settingspath;
    static Settings settings;

    static inline std::string version;

    static inline HMODULE currentModule = NULL;

    static void SaveSettings() {
        try {
            std::ofstream outputFile(settingspath);
            if (outputFile) {
                outputFile << settings.ToJson();
            } else {
                Logger::error("Failed to open file for saving settings: {}", settingspath);
            }
        } catch (const std::exception& e) {
            Logger::error("An error occurred while trying to save settings to {}: {}", settingspath, e.what());
        }
    }

    static void LoadSettings() {
        std::ifstream inputFile(settingspath);

        if (!inputFile) {
            Logger::error("Failed to open settings file for loading: {}", settingspath);
            return;
        }

        std::stringstream ss;
        ss << inputFile.rdbuf();
        settings.FromJson(ss.str());
    }

    static void CheckSettingsFile() {
        if (!std::filesystem::exists(settingspath)) {
            std::filesystem::create_directories(std::filesystem::path(settingspath).parent_path());

            std::ofstream file(settingspath, std::ios::app);
            if (!file) {
                Logger::error("Failed to create settings file: {}", settingspath);
            }
        }
    }
};
