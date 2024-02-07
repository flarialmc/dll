#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <sstream>
#include "../../GUI/Engine/Engine.hpp"
#include "../../GUI/Engine/Constraints.hpp"
#include "../../../SDK/SDK.hpp"

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

        CheckSettingsFile();
        LoadSettings();

        if(settings.getSettingByName<bool>("enabled") == nullptr) {
            settings.addSetting("enabled", false);
        }

        if(settings.getSettingByName<std::string>("keybind") == nullptr) {
            settings.addSetting("keybind", (std::string)"");
        }

        enabled = settings.getSettingByName<bool>("enabled");
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

        DefaultConfig();

    }

    virtual void onDisable() {

        SaveSettings();

    }

    virtual void DefaultConfig() {
       if(settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

       if(settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", false);
            settings.addSetting("borderWidth", 1.0f);
        }

        if (settings.getSettingByName<bool>("reversepaddingx") == nullptr) settings.addSetting("reversepaddingx", false);
        if (settings.getSettingByName<bool>("reversepaddingy") == nullptr) settings.addSetting("reversepaddingy", false);
        if (settings.getSettingByName<float>("padx") == nullptr) settings.addSetting("padx", 0.0f);
        if (settings.getSettingByName<float>("pady") == nullptr) settings.addSetting("pady", 0.0f);
        if (settings.getSettingByName<float>("rectwidth") == nullptr) settings.addSetting("rectwidth", 1.0f);
        if (settings.getSettingByName<float>("rectheight") == nullptr) settings.addSetting("rectheight", 1.0f);
        if (settings.getSettingByName<bool>("responsivewidth") == nullptr) settings.addSetting("responsivewidth", false);
        if (settings.getSettingByName<std::string>("textalignment") == nullptr) settings.addSetting("textalignment", (std::string)"Center");

        if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 32.0f);

        if (settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string)"000000");
            settings.addSetting("textColor", (std::string)"fafafa");
            settings.addSetting("borderColor", (std::string)"000000");
        }

        if (settings.getSettingByName<float>("bgOpacity") == nullptr) {
            settings.addSetting("bgOpacity", 0.55f);
            settings.addSetting("textOpacity", 1.0f);
            settings.addSetting("borderOpacity", 1.0f);
        }

        if (settings.getSettingByName<bool>("bgRGB") == nullptr) {
            settings.addSetting("bgRGB", false);
            settings.addSetting("textRGB", false);
            settings.addSetting("borderRGB", false);
        }

       if(settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }

        if (settings.getSettingByName<float>("rotation") == nullptr) {
            settings.addSetting("rotation", 0.0f);
        }

        if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
            settings.addSetting("BlurEffect", false);
        }

    }

    virtual void SettingsRender() {}

    bool IsKeybind(const std::array<bool, 256>& keys) {

        std::vector<int> keyCodes = Utils::GetStringAsKeys(settings.getSettingByName<std::string>("keybind")->value);

        for (int keyCode : keyCodes) {
            if (!keys[keyCode]) {
                // Key is not being held down
                return false;
            }
        }
        if (FlarialGUI::inMenu) return false;
        for (TextBoxStruct& i : FlarialGUI::TextBoxes) if (i.isActive) return false;
        if (SDK::CurrentScreen == "chat_screen") return false;
        // All keys in the keybind are being held down
        return true;
    }

    bool IsAllahKeybind(const std::array<bool, 256>& keys, std::string bind) {

        std::vector<int> keyCodes = Utils::GetStringAsKeys(bind);

        for (int keyCode : keyCodes) {
            if (!keys[keyCode]) {

                // Key is not being held down
                return false;
            }
        }

        if (FlarialGUI::inMenu) return false;
        for (TextBoxStruct& i : FlarialGUI::TextBoxes) if (i.isActive) return false;
        // All keys in the keybind are being held down
        if (SDK::CurrentScreen == "chat_screen") return false;
        return true;
    }

    bool IsKeyPartOfKeybind(int keyCode) {
        std::vector<int> keyCodes = Utils::GetStringAsKeys(settings.getSettingByName<std::string>("keybind")->value);
        return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
    }

    bool IsKeyPartOfAllahKeybind(int keyCode, std::string bind) {
        std::vector<int> keyCodes = Utils::GetStringAsKeys(bind);
        return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
    }

    virtual void NormalRender(int index, std::string text, std::string value);
};