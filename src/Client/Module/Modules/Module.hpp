#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>

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

void SaveSettings() {
    try {
        // Open the file for writing
        std::ofstream outputFile(settingspath);


        json jsonData = settings.ToJson();
        if (outputFile.is_open()) {
            // Write the JSON to the file
            outputFile << settings.ToJson();  // Indent with 4 spaces for readability
            outputFile.close();
        } else {
            std::cerr << "Failed to open file: " << settingsPath << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Exception occurred while saving settings: " << ex.what() << std::endl;
    }
}


virtual void onEnable() {}

    virtual void onDisable() {

        SaveSettings();

   }
};