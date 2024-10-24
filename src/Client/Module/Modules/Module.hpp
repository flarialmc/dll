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
#include <vector>
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
    bool enabledState = false;
    bool restricted = false;
    bool terminating = false;

    float padding = 0;
    int toggleIndex = 0;
    int sliderIndex = 0;
    std::vector<float> conditionalSliderAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));
    int dropdownIndex = 0;
    int textboxIndex = 0;
    int keybindIndex = 0;
    int colorPickerIndex = 100;

    struct DrDisrespect {
        std::string* value;
        float* opacity;
        bool* rgb;
    };

    std::unordered_map<int, DrDisrespect> color_pickers;



    void resetPadding();
    void extraPadding();
    void addElementText(std::string text, std::string subtext = "");
    void addHeader(std::string text);
    void addConditionalSlider(bool condition, std::string text, std::string subtext, float& value, float maxVal = 100.0f, float minVal = 0.0f, bool zerosafe = true);
    void addSlider(std::string text, std::string subtext, float& value, float maxVal = 100.0f, float minVal = 0.0f, bool zerosafe = true);
    void addToggle(std::string text, std::string subtext, bool& value);

    void addKeybind(std::string text, std::string subtext, std::string &keybind);

    void addTextBox(std::string text, std::string subtext, std::string& value);
    void addDropdown(std::string text, std::string subtext, const std::vector<std::string>& options, std::string& value);
    void addColorPicker(std::string text, std::string subtext, std::string& value, float& opacity, bool& rgb);
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