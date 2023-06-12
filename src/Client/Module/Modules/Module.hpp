#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <sstream>


class Module
{
public:
    std::string name;
    std::string description;
    std::string icon;
    Settings settings;
    std::string settingspath;

    int keybind;

    Module(const std::string& ename, const std::string& edescription, const std::string& eicon, int ekey) {
        name = ename;
        description = edescription;
        icon = eicon;
        keybind = ekey;
        settings = Settings();
        settingspath = Utils::getRoamingPath() + "\\Flarial\\Config\\" + name + ".flarial";
        LoadSettings();
    }
public:
    bool enabled = false;

public:

    void SaveSettings() const {
        try {
            std::ofstream outputFile(settingspath);
            if (outputFile.is_open()) {
                std::string jsonString = settings.ToJson();
                outputFile << jsonString;
                outputFile.close();
            } else {
                Logger::error("Failed to open file. Maybe it doesn't exist?: " + settingspath);
            }
        } catch (const std::exception& ex) {
            Logger::error(ex.what());
        }
    }

    void LoadSettings() {
        std::ifstream inputFile(settingspath);
        std::stringstream ss;

        if (inputFile.is_open()) {
            ss << inputFile.rdbuf();
            inputFile.close();
        } else {
            Logger::error("File could not be opened. Maybe it doesn't exist?: " + settingspath);
            return;
        }

        std::string settingstring = ss.str();
        settings.FromJson(settingstring);
    }

    void CheckSettingsFile() const {

        if (!std::filesystem::exists(settingspath)) {

            std::filesystem::path filePath(settingspath);
            std::filesystem::create_directories(filePath.parent_path());

            HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (fileHandle == INVALID_HANDLE_VALUE) {
                Logger::error("Failed to create file: " + settingspath);
                return;
            }

            CloseHandle(fileHandle);
        }
    }


   virtual void onEnable() {

    CheckSettingsFile();
}

    virtual void onDisable() {

        SaveSettings();

   }
};