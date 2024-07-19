#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <utility>
#include "../../GUI/Engine/Engine.hpp"
#include "../../GUI/Engine/Constraints.hpp"
#include "../../../SDK/SDK.hpp"
#include "../../../Assets/Assets.hpp"

class Module {
public:
    std::string name;
    std::string description;
    int icon;
    std::string defaultKeybind;
    Settings settings;
    std::string settingspath;

    Module(const std::string &ename, const std::string &edescription, int eicon, const std::string& ekey) {
        name = ename;
        description = edescription;
        icon = eicon;
        defaultKeybind = ekey;
        settings = Settings();
        settingspath = Utils::getRoamingPath() + R"(\Flarial\Config\)" + name + ".flarial";

        checkSettingsFile();
        loadSettings();
    }

public:
    bool active = false;
    bool restricted = false;
    bool terminating = false;

public:

    virtual void loadDefaults() {
        settings.reset();
        setup();
    }

    void saveSettings() const {
        try {
            std::ofstream outputFile(settingspath);
            if (outputFile.is_open()) {
                std::string jsonString = settings.ToJson();
                outputFile << jsonString;
                outputFile.close();
            } else {
                Logger::error("Failed to open file. Maybe it doesn't exist?: " + settingspath);
            }
        } catch (const std::exception &ex) {
            Logger::error(ex.what());
        }
    }

    void loadSettings() {
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

    void checkSettingsFile() const {

        if (!std::filesystem::exists(settingspath)) {

            std::filesystem::path filePath(settingspath);
            std::filesystem::create_directories(filePath.parent_path());

            HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (fileHandle == INVALID_HANDLE_VALUE) {
                Logger::error("Failed to create file: " + settingspath);
                return;
            }

            CloseHandle(fileHandle);
        }
    }

    virtual void toggle() {
        bool &enabled = settings.getSettingByName<bool>("enabled")->value;
        enabled = !enabled;
        if (enabled) {
            onEnable();
        } else {
            onDisable();
        }
    }

    virtual void setup() {
        defaultConfig();
        Module::defaultConfig();
        onSetup();
        // TODO: might call on enable twice
        if(settings.getSettingByName<bool>("enabled")->value)
            onEnable();
    }

    virtual void onSetup() { }

    // TODO: rename to Enable/Disable?
    virtual void onEnable() {
        settings.getSettingByName<bool>("enabled")->value = true;
        saveSettings();
    }

    virtual void onDisable() {
        active = false;
        if(!terminating) {
            settings.getSettingByName<bool>("enabled")->value = false;
        }
        saveSettings();
    }

    virtual void terminate() {
        terminating = true;
        onDisable();
        active = false;
        settings.getSettingByName<bool>("enabled")->value = false;
    }

    // TODO: find all getSettingByName<bool>("enabled")->value and replace!!!
    bool isEnabled() {
        return settings.getSettingByName<bool>("enabled")->value;
    }

    void setEnabled(bool enabled) {
        settings.getSettingByName<bool>("enabled")->value = enabled;
    }

    void setKeybind(const std::string& newKeybind) {
        auto key = settings.getSettingByName<std::string>("keybind");
        if(key == nullptr)
            settings.addSetting("keybind", newKeybind);
    }

    std::string& getKeybind() {
        auto key = settings.getSettingByName<std::string>("keybind");
        if(key == nullptr)
            settings.addSetting("keybind", defaultKeybind);
        return settings.getSettingByName<std::string>("keybind")->value;
    }

    virtual void defaultConfig() {

        getKeybind();

        if (settings.getSettingByName<bool>("enabled") == nullptr) {
            settings.addSetting("enabled", false);
        }

        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if (settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", false);
            settings.addSetting("borderWidth", 1.0f);
        }

        if (settings.getSettingByName<bool>("reversepaddingx") == nullptr)
            settings.addSetting("reversepaddingx", false);
        if (settings.getSettingByName<bool>("reversepaddingy") == nullptr)
            settings.addSetting("reversepaddingy", false);
        if (settings.getSettingByName<float>("padx") == nullptr) settings.addSetting("padx", 0.0f);
        if (settings.getSettingByName<float>("pady") == nullptr) settings.addSetting("pady", 0.0f);
        if (settings.getSettingByName<float>("rectwidth") == nullptr) settings.addSetting("rectwidth", 1.0f);
        if (settings.getSettingByName<float>("rectheight") == nullptr) settings.addSetting("rectheight", 1.0f);
        if (settings.getSettingByName<bool>("responsivewidth") == nullptr)
            settings.addSetting("responsivewidth", false);
        if (settings.getSettingByName<std::string>("textalignment") == nullptr)
            settings.addSetting("textalignment", (std::string) "Center");

        if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 32.0f);

        if (settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string) "000000");
            settings.addSetting("textColor", (std::string) "fafafa");
            settings.addSetting("borderColor", (std::string) "000000");
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

        if (settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }

        if (settings.getSettingByName<float>("rotation") == nullptr) {
            settings.addSetting("rotation", 0.0f);
        }

        if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
            settings.addSetting("BlurEffect", false);
        }

    }

    virtual void settingsRender() {}

    bool isKeybind(const std::array<bool, 256> &keys) {

        std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind());

        for (int keyCode: keyCodes) {
            if (!keys[keyCode]) {
                // Key is not being held down
                return false;
            }
        }
        // TODO: in menu should also mean in Edit Mode, when edit mode all modules set to active(visible? add visible setting (for TAB))
        if(name!="ClickGUI")
            if (FlarialGUI::inMenu) return false;
        if (SDK::currentScreen == "chat_screen") return false;
        bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const TextBoxStruct& i) {
            return !i.isActive;
        });
        // All keys in the keybind are being held down
        return allInactive;
    }

    [[nodiscard]] bool isAdditionalKeybind(const std::array<bool, 256> &keys, const std::string& bind) const {
        std::vector<int> keyCodes = Utils::getStringAsKeys(bind);

        for (int keyCode: keyCodes) {
            if (!keys[keyCode]) {

                // Key is not being held down
                return false;
            }
        }
        if(name!="ClickGUI")
            if (FlarialGUI::inMenu) return false;
        if (SDK::currentScreen == "chat_screen") return false;
        bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const TextBoxStruct& i) {
            return !i.isActive;
        });
        // All keys in the keybind are being held down
        return allInactive;
    }

    bool isKeyPartOfKeybind(int keyCode) {
        std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind());
        return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
    }

    static bool isKeyPartOfAdditionalKeybind(int keyCode, const std::string& bind) {
        std::vector<int> keyCodes = Utils::getStringAsKeys(bind);
        return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
    }

    virtual void normalRender(int index, std::string& value);
};