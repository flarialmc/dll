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

    bool active = false;
    bool restricted = false;
    bool terminating = false;

    float padding = 0;
    int toggleIndex = 0;
    int sliderIndex = 0;
    int dropdownIndex = 0;
    int textboxIndex = 0;
    int keybindIndex = 0;
    int colorPickerIndex = 0;

    void resetPadding();

    void extraPadding();

    void addHeader(std::string text);
    void addToggle(std::string text, std::string subtext, bool& value);
    virtual void loadDefaults();
    void saveSettings() const;
    void loadSettings();
    void checkSettingsFile() const;
    virtual void toggle();
    virtual void setup();
    virtual void onSetup();
    virtual void onEnable();
    virtual void onDisable();
    virtual void terminate();
    bool isEnabled();
    void setEnabled(bool enabled);
    void setKeybind(const std::string& newKeybind);
    std::string& getKeybind();
    virtual void defaultConfig();
    virtual void settingsRender() {}
    bool isKeybind(const std::array<bool, 256>& keys);
    [[nodiscard]] bool isAdditionalKeybind(const std::array<bool, 256>& keys, const std::string& bind) const;
    bool isKeyPartOfKeybind(int keyCode);
    static bool isKeyPartOfAdditionalKeybind(int keyCode, const std::string& bind);
    virtual void normalRender(int index, std::string& value);
};