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

    virtual void NormalRender() {

        float textWidth = Constraints::RelativeConstraint(0.33);
        float textHeight = Constraints::RelativeConstraint(0.1);


        Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                              this->settings.getSettingByName<float>("percentageY")->value);

        Vec2<float> realcenter;

        if (settingperc.x != 0)
            realcenter = Vec2<float>(Constraints::PercentageConstraint(settingperc.x, "left"),
                                     Constraints::PercentageConstraint(settingperc.y, "top"));
        else
            realcenter = Constraints::CenterConstraint(textWidth, textHeight);

        float rectWidth = Constraints::RelativeConstraint(0.225);
        Vec2<float> rounde = Constraints::RoundingConstraint(40, 40);

        float realspacing = Constraints::SpacingConstraint(0.33, textWidth);

        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, textHeight, 0);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, 0);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;


        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);



        FlarialGUI::RoundedRect(realcenter.x, realcenter.y,
                                D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), rectWidth, textHeight,
                                rounde.x, rounde.x);
        FlarialGUI::FlarialText(realcenter.x - realspacing, realcenter.y,
                                FlarialGUI::to_wide(std::format("FPS: {}", MC::fps)).c_str(),
                                D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), textWidth,
                                textHeight);

        if(this->settings.getSettingByName<bool>("border")->value) {
            FlarialGUI::RoundedHollowRect(realcenter.x, realcenter.y, this->settings.getSettingByName<float>("borderWidth")->value,
                                    D2D1::ColorF(D2D1::ColorF::Black), rectWidth, textHeight,
                                    rounde.x, rounde.x);
        }

        FlarialGUI::UnsetWindowRect();

    }

    virtual void SettingsRender() {}

};