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
            if (outputFile.is_open()) {
                std::string jsonString = settings.ToJson();
                outputFile << jsonString;
                outputFile.close();
            } else {
                Logger::error("Failed to save open file. Maybe it doesn't exist?: {}", settingspath);
            }
        } catch (const std::exception &ex) {
            Logger::error("An error occurred while trying to save settings: {}", ex.what());
        }
    }

    static void LoadSettings() {
        std::ifstream inputFile(settingspath);
        std::stringstream ss;

        if (inputFile.is_open()) {
            ss << inputFile.rdbuf();
            inputFile.close();
        } else {
            Logger::error("Failed to save open file. Maybe it doesn't exist?: {}", settingspath);
            return;
        }

        std::string settingstring = ss.str();
        settings.FromJson(settingstring);
    }

    static void CheckSettingsFile() {

        if (!std::filesystem::exists(settingspath)) {

            std::filesystem::path filePath(settingspath);
            std::filesystem::create_directories(filePath.parent_path());

            HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (fileHandle == INVALID_HANDLE_VALUE) {
                Logger::error("Failed to create file: {}", settingspath);
                return;
            }

            CloseHandle(fileHandle);
        }
    }
};