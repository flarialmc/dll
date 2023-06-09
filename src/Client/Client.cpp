#pragma once
#include "Client.hpp"
#include "../Utils/Logger/Logger.hpp"
#include "GUI/Engine/Engine.hpp"
#include <kiero.h>
#include <filesystem>

bool Client::disable = false;

void Client::initialize()
{
    std::filesystem::path folder_path(Utils::getRoamingPath() + "\\Flarial");
    if (!exists(folder_path))
    {
        create_directory(folder_path);
    }

    std::filesystem::path folder_path2(Utils::getRoamingPath() + "\\Flarial\\assets");
    if (!exists(folder_path2))
    {
        create_directory(folder_path2);
    }

    std::filesystem::path folder_path3(Utils::getRoamingPath() + "\\Flarial\\logs");
    if (!exists(folder_path3))
    {
        create_directory(folder_path3);
    }

    std::string dwnld_URL = "https://cdn.flarial.net/assets/gear.png";
    std::string savepath = Utils::getRoamingPath() + "\\Flarial\\assets\\gear.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL).c_str(), FlarialGUI::to_wide(savepath).c_str(), 0, NULL);

    std::string dwnld_URL2 = "https://cdn.flarial.net/assets/logo.png";
    std::string savepath2 = Utils::getRoamingPath() + "\\Flarial\\assets\\logo.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL2).c_str(), FlarialGUI::to_wide(savepath2).c_str(), 0, NULL);

    std::string dwnld_URL3 = "https://cdn.flarial.net/assets/fps.png";
    std::string savepath3 = Utils::getRoamingPath() + "\\Flarial\\assets\\fps.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL3).c_str(), FlarialGUI::to_wide(savepath3).c_str(), 0, NULL);

    std::string dwnld_URL4 = "https://cdn.flarial.net/assets/autogg.png";
    std::string savepath4 = Utils::getRoamingPath() + "\\Flarial\\assets\\autogg.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL4).c_str(), FlarialGUI::to_wide(savepath4).c_str(), 0, NULL);

    std::string dwnld_URL5 = "https://cdn.flarial.net/assets/autosprint.png";
    std::string savepath5 = Utils::getRoamingPath() + "\\Flarial\\assets\\autosprint.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL5).c_str(), FlarialGUI::to_wide(savepath5).c_str(), 0, NULL);

    std::string dwnld_URL6 = "https://cdn.flarial.net/assets/clickgui.png";
    std::string savepath6 = Utils::getRoamingPath() + "\\Flarial\\assets\\clickgui.png";
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(dwnld_URL6).c_str(), FlarialGUI::to_wide(savepath6).c_str(), 0, NULL);

    Logger::initialize();

    HookManager::initialize();
    ModuleManager::initialize();
}
