#pragma once
#include "Client.hpp"
#include "GUI/Engine/Engine.hpp"
#include <kiero.h>
#include <filesystem>
#include <thread>
#include <wingdi.h>
#include <wininet.h>

std::string Client::settingspath = Utils::getRoamingPath() + "\\Flarial\\Config\\main.flarial";
Settings Client::settings = Settings();

void DownloadAndSave(std::string url, std::string path) {

    char test[256];
    strcpy(test, "https://google.com/");
    if(InternetCheckConnectionA(test, FLAG_ICC_FORCE_CONNECTION, 0))
    URLDownloadToFileW(NULL, FlarialGUI::to_wide(url).c_str(), FlarialGUI::to_wide(path).c_str(), 0, NULL);

}

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

    std::filesystem::path folder_path4(Utils::getRoamingPath() + "\\Flarial\\Config");
    if (!exists(folder_path4))
    {
        create_directory(folder_path4);
    }

    std::string Path = Utils::getRoamingPath() + "\\Flarial\\assets\\";

    std::pair<std::string, std::string> fileData[] = {
        { "https://cdn.flarial.net/assets/gear.png", Path + "gear.png" },
        { "https://cdn.flarial.net/assets/font.ttf", Path + "font.ttf" },
        { "https://cdn.flarial.net/assets/logo.png", Path + "logo.png" },
        { "https://cdn.flarial.net/assets/fps.png", Path + "fps.png" },
        { "https://cdn.flarial.net/assets/autogg.png", Path + "autogg.png" },
        { "https://cdn.flarial.net/assets/auto_sprint.png", Path + "auto_sprint.png" },
        { "https://cdn.flarial.net/assets/clickgui.png", Path + "clickgui.png" },
        { "https://cdn.flarial.net/assets/modules.png", Path + "modules.png" },
        { "https://cdn.flarial.net/assets/cursor.png", Path + "cursor.png" },
        { "https://cdn.flarial.net/assets/magnify.png", Path + "magnify.png" },
        { "https://cdn.flarial.net/assets/memory.png", Path + "memory.png" },
        { "https://cdn.flarial.net/assets/time.png", Path + "time.png" },
        { "https://cdn.flarial.net/assets/fullbright.png", Path + "fullbright.png" },
        { "https://cdn.flarial.net/assets/frying-pan.png", Path + "frying-pan.png" },
        { "https://cdn.flarial.net/assets/font_bold.ttf", Path + "font_bold.ttf" },
        { "https://cdn.flarial.net/assets/flarial-title.png", Path + "flarial-title.png" },
        { "https://cdn.flarial.net/assets/combo.png", Path + "combo.png" },
        { "https://cdn.flarial.net/assets/reach.png", Path + "reach.png" },
        { "https://cdn.flarial.net/assets/keyboard.png", Path + "keyboard.png" },
        { "https://cdn.flarial.net/assets/server-ip.png", Path + "server-ip.png" },
        { "https://cdn.flarial.net/assets/coordinates.png", Path + "coordinates.png" },
        { "https://cdn.flarial.net/assets/ping.png", Path + "ping.png" },
        { "https://cdn.flarial.net/assets/arrow.png", Path + "arrow.png" },
        { "https://cdn.flarial.net/assets/potion.png", Path + "potion.png" },
        { "https://cdn.flarial.net/assets/nametag.png", Path + "nametag.png" },
        { "https://cdn.flarial.net/assets/minecraftia.ttf", Path + "minecraftia.ttf" },
        { "https://cdn.flarial.net/assets/blur.png", Path + "blur.png" },
        { "https://cdn.flarial.net/assets/eye.png", Path + "eye.png" },
        { "https://cdn.flarial.net/assets/chestplate.png", Path + "chestplate.png" },
        { "https://cdn.flarial.net/assets/patar.jpg", Path + "patar.jpg" },
        { "https://cdn.flarial.net/assets/skull.png", Path + "skull.png" },
        { "https://cdn.flarial.net/assets/freelook.png", Path + "freelook.png" },
        { "https://cdn.flarial.net/assets/hurt.png", Path + "hurt.png" },
        { "https://cdn.flarial.net/assets/smoke.png", Path + "smoke.png" },
        { "https://cdn.flarial.net/assets/renderoptions.png", Path + "renderoptions.png" },
        { "https://cdn.flarial.net/assets/man.png", Path + "man.png" }



    };

    Client::CheckSettingsFile();
    Client::LoadSettings();

    if(Client::settings.getSettingByName<std::string>("fontname") == nullptr)
        Client::settings.addSetting("fontname", (std::string)"Space Grotesk");

    if(Client::settings.getSettingByName<float>("blurintensity") == nullptr)
        Client::settings.addSetting("blurintensity", 18.0f);

    if(Client::settings.getSettingByName<bool>("killdx") == nullptr)
        Client::settings.addSetting("killdx", false);

    if (Client::settings.getSettingByName<bool>("vsync") == nullptr)
        Client::settings.addSetting("vsync", false);

    // Create threads to download the files
    std::vector<std::thread> threads;
    for (const auto& data : fileData) {
        threads.emplace_back(DownloadAndSave, data.first, data.second);
    }

    // Wait for all threads to finish
    for (std::thread& thread : threads) {
        thread.join();
    }

    std::string fontpath = Utils::getRoamingPath() + "\\Flarial\\assets\\font.ttf";
    AddFontResource(fontpath.c_str());

    fontpath = Utils::getRoamingPath() + "\\Flarial\\assets\\font_bold.ttf";
    AddFontResource(fontpath.c_str());

    fontpath = Utils::getRoamingPath() + "\\Flarial\\assets\\minecraftia.ttf";
    AddFontResource(fontpath.c_str());

    Logger::initialize();

    ModuleManager::initialize();

    Sleep(1000);

    HookManager::initialize();

    Sleep(1000);

    FlarialGUI::Notify("Report bugs at https://flarial.net/discord!");
    FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("keybind")->value + " to open the menu in-game.");

}
