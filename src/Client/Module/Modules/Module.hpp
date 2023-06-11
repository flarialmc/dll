#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <collection.h>

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
        settingspath = Utils::getRoamingPath() + "\\Config\\" + name + ".flarial";
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

        std::filesystem::path filePath(settingspath);

        if (!std::filesystem::exists(filePath)) {
            std::ofstream outputFile(filePath);
            if (!outputFile.is_open()) {
                // Handle file creation error
                return;
            }
            outputFile.close();
        }
    }


   virtual void onEnable() {

    CheckSettingsFile();

}

    virtual void onDisable() {

        SaveSettings();

   }
};