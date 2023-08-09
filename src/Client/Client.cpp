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

    if(Client::settings.getSettingByName<bool>("dlassets")->value || !std::filesystem::exists(path)) {
        char test[256];
        strcpy(test, "https://cdn-c6f.pages.dev/");
        if (InternetCheckConnectionA(test, FLAG_ICC_FORCE_CONNECTION, 0))
            URLDownloadToFileW(NULL, FlarialGUI::to_wide(url).c_str(), FlarialGUI::to_wide(path).c_str(), 0, NULL);
    }

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
        { "https://cdn-c6f.pages.dev/assets/gear.png", Path + "gear.png" },
        { "https://cdn-c6f.pages.dev/assets/font.ttf", Path + "font.ttf" },
        { "https://cdn-c6f.pages.dev/assets/logo.png", Path + "logo.png" },
        { "https://cdn-c6f.pages.dev/assets/fps.png", Path + "fps.png" },
        { "https://cdn-c6f.pages.dev/assets/autogg.png", Path + "autogg.png" },
        { "https://cdn-c6f.pages.dev/assets/auto_sprint.png", Path + "auto_sprint.png" },
        { "https://cdn-c6f.pages.dev/assets/clickgui.png", Path + "clickgui.png" },
        { "https://cdn-c6f.pages.dev/assets/modules.png", Path + "modules.png" },
        { "https://cdn-c6f.pages.dev/assets/cursor.png", Path + "cursor.png" },
        { "https://cdn-c6f.pages.dev/assets/magnify.png", Path + "magnify.png" },
        { "https://cdn-c6f.pages.dev/assets/memory.png", Path + "memory.png" },
        { "https://cdn-c6f.pages.dev/assets/time.png", Path + "time.png" },
        { "https://cdn-c6f.pages.dev/assets/fullbright.png", Path + "fullbright.png" },
        { "https://cdn-c6f.pages.dev/assets/frying-pan.png", Path + "frying-pan.png" },
        { "https://cdn-c6f.pages.dev/assets/font_bold.ttf", Path + "font_bold.ttf" },
        { "https://cdn-c6f.pages.dev/assets/flarial-title.png", Path + "flarial-title.png" },
        { "https://cdn-c6f.pages.dev/assets/combo.png", Path + "combo.png" },
        { "https://cdn-c6f.pages.dev/assets/reach.png", Path + "reach.png" },
        { "https://cdn-c6f.pages.dev/assets/keyboard.png", Path + "keyboard.png" },
        { "https://cdn-c6f.pages.dev/assets/server-ip.png", Path + "server-ip.png" },
        { "https://cdn-c6f.pages.dev/assets/coordinates.png", Path + "coordinates.png" },
        { "https://cdn-c6f.pages.dev/assets/ping.png", Path + "ping.png" },
        { "https://cdn-c6f.pages.dev/assets/arrow.png", Path + "arrow.png" },
        { "https://cdn-c6f.pages.dev/assets/potion.png", Path + "potion.png" },
        { "https://cdn-c6f.pages.dev/assets/nametag.png", Path + "nametag.png" },
        { "https://cdn-c6f.pages.dev/assets/minecraftia.ttf", Path + "minecraftia.ttf" },
        { "https://cdn-c6f.pages.dev/assets/blur.png", Path + "blur.png" },
        { "https://cdn-c6f.pages.dev/assets/eye.png", Path + "eye.png" },
        { "https://cdn-c6f.pages.dev/assets/chestplate.png", Path + "chestplate.png" },
        { "https://cdn-c6f.pages.dev/assets/patar.jpg", Path + "patar.jpg" },
        { "https://cdn-c6f.pages.dev/assets/skull.png", Path + "skull.png" },
        { "https://cdn-c6f.pages.dev/assets/freelook.png", Path + "freelook.png" },
        { "https://cdn-c6f.pages.dev/assets/hurt.png", Path + "hurt.png" },
        { "https://cdn-c6f.pages.dev/assets/smoke.png", Path + "smoke.png" },
        { "https://cdn-c6f.pages.dev/assets/renderoptions.png", Path + "renderoptions.png" },
        { "https://cdn-c6f.pages.dev/assets/man.png", Path + "man.png" },
        { "https://cdn-c6f.pages.dev/assets/search.png", Path + "search.png" },
        { "https://cdn-c6f.pages.dev/assets/slowly.png", Path + "slowly.png" },
        { "https://cdn-c6f.pages.dev/assets/cloudy.png", Path + "cloudy.png" },
        { "https://cdn-c6f.pages.dev/assets/list.png", Path + "list.png" }



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

    if (Client::settings.getSettingByName<bool>("dlassets") == nullptr)
        Client::settings.addSetting("dlassets", true);

    if (Client::settings.getSettingByName<bool>("noicons") == nullptr)
        Client::settings.addSetting("noicons", false);

    if (Client::settings.getSettingByName<bool>("noshadows") == nullptr)
        Client::settings.addSetting("noshadows", false);

    if (Client::settings.getSettingByName<bool>("watermark") == nullptr)
        Client::settings.addSetting("watermark", true);



        // Create threads to download the files
        std::vector<std::thread> threads;
        for (const auto &data: fileData) {
            threads.emplace_back(DownloadAndSave, data.first, data.second);
        }

        // Wait for all threads to finish
        for (std::thread &thread: threads) {
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

    Sleep(2500);

    FlarialGUI::Notify("Report bugs at https://flarial.net/discord!");
    FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("keybind")->value + " to open the menu in-game.");

}
