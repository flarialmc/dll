#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <filesystem>

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
        settings.addSetting("sussy", 0);
    }
public:
    bool enabled = false;

public:

void SaveSettings() const {
    try {

        std::ofstream outputFile(settingspath);


        json jsonData = settings.ToJson();
        if (outputFile.is_open()) {

            outputFile << settings.ToJson();
            outputFile.close();
        } else {
            Logger::error("Failed to open file. Maybe it doesn't exist?: " + settingspath);
        }
    } catch (const std::exception& ex) {
        Logger::error(ex.what());
    }
}

    void LoadSettings() const {
        std::ifstream inputFile(settingspath);
        std::string settingstring;

        if (inputFile.is_open()) {
            std::string line;

            while (std::getline(inputFile, line)) {
                settingstring += line;
            }

            settings.FromJson(settingstring);

            inputFile.close();
        } else {
            Logger::error("File could not be opened. Maybe it doesn't exist?: " + settingspath);
        }


    }

    void CheckSettingsFile() const {

        if (!std::filesystem::exists(settingspath)) {

            std::filesystem::path filePath(settingspath);
            std::filesystem::create_directories(filePath.parent_path());

            HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (fileHandle == INVALID_HANDLE_VALUE) {
                Logger::debug("Failed to create file: " + settingspath);
                return;
            }

            CloseHandle(fileHandle);
        } else {

            Logger::debug("Exists");

        }
    }


   virtual void onEnable() {

    Logger::debug(settingspath);
    CheckSettingsFile();

}

    virtual void onDisable() {

        SaveSettings();

   }
};