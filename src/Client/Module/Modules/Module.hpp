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

}

    virtual void onDisable() {

        SaveSettings();

   }

    virtual void NormalRender(int index, std::string text, std::string value) {

        std::string uppercaseSentence;
        std::string search = "{VALUE}";

        for (char c : text) {
            uppercaseSentence += std::toupper(c);
        }

        size_t pos = uppercaseSentence.find(search);
        if (pos != std::string::npos) {
            text.replace(pos, search.length(), value);
        }

        float textWidth = Constraints::RelativeConstraint(0.33f * settings.getSettingByName<float>("uiscale")->value);
        float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);


        Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                              this->settings.getSettingByName<float>("percentageY")->value);

        Vec2<float> realcenter;

        if (settingperc.x != 0)
            realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
                                     settingperc.y * MC::windowSize.y);
        else
            realcenter = Constraints::CenterConstraint(textWidth, textHeight);

        float rectWidth = Constraints::RelativeConstraint(0.225f * settings.getSettingByName<float>("uiscale")->value);
        Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value * settings.getSettingByName<float>("uiscale")->value, this->settings.getSettingByName<float>("rounding")->value * settings.getSettingByName<float>("uiscale")->value);

        float realspacing = Constraints::SpacingConstraint(0.155f, textWidth);

        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, textHeight, index);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, rectWidth, textHeight);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;


        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        D2D1_COLOR_F bgColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
        D2D1_COLOR_F textColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
        D2D1_COLOR_F borderColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("borderColor")->value);

        bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
        textColor.a = settings.getSettingByName<float>("textOpacity")->value;
        borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

        if(settings.getSettingByName<bool>("BlurEffect") != nullptr)

        if(settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + rectWidth, realcenter.y + textHeight), rounde.x, rounde.x), settings.getSettingByName<float>("BlurIntensity")->value);

        FlarialGUI::RoundedRect(realcenter.x, realcenter.y,
                                bgColor, rectWidth, textHeight,
                                rounde.x, rounde.x);
        FlarialGUI::FlarialTextWithFont(realcenter.x - realspacing, realcenter.y,
                                FlarialGUI::to_wide(text).c_str(),
                                textColor, textWidth,
                                textHeight, DWRITE_TEXT_ALIGNMENT_CENTER, 190 * settings.getSettingByName<float>("textscale")->value * settings.getSettingByName<float>("uiscale")->value );

        if(this->settings.getSettingByName<bool>("border")->value) {
            FlarialGUI::RoundedHollowRect(realcenter.x, realcenter.y, Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100, "height", true),
                                          borderColor, rectWidth, textHeight,
                                    rounde.x, rounde.x);
        }

        FlarialGUI::UnsetWindowRect();

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
        // All keys in the keybind are being held down
        return true;
    }

    bool IsKeyPartOfKeybind(int keyCode) {
        std::vector<int> keyCodes = Utils::GetStringAsKeys(settings.getSettingByName<std::string>("keybind")->value);
        return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
    }


};