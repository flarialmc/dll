#pragma once
#include "Client.hpp"
#include "GUI/Engine/Engine.hpp"
#include <kiero.h>
#include <filesystem>
#include <thread>
#include <wingdi.h>

void DownloadAndSave(std::string url, std::string path) {
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
        { "https://cdn.flarial.net/assets/font.ttf", Path + "font.png" },
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
    };

    // Create threads to download the files
    std::vector<std::thread> threads;
    for (const auto& data : fileData) {
        threads.emplace_back(DownloadAndSave, data.first, data.second);
    }

    // Wait for all threads to finish
    for (std::thread& thread : threads) {
        thread.join();
    }

    if (GetModuleHandle("d3d12.dll") == NULL) Logger::debug("module handle is null");

    std::string fontpath = Utils::getRoamingPath() + "\\Flarial\\assets\\font.ttf";
    AddFontResource(fontpath.c_str());

    fontpath = Utils::getRoamingPath() + "\\Flarial\\assets\\font_bold.ttf";
    AddFontResource(fontpath.c_str());

    Logger::initialize();

    ModuleManager::initialize();

    Sleep(1000);

    HookManager::initialize();

    FlarialGUI::Notify("Injected");
    FlarialGUI::Notify("Injected");

}
