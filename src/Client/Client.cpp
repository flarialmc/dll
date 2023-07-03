#pragma once
#include "Client.hpp"
#include "GUI/Engine/Engine.hpp"
#include <kiero.h>
#include <filesystem>
#include <wingdi.h>

bool Client::disable = false;

void Client::initialize()
{
    Logger::initialize();

    ModuleManager::initialize();

    Sleep(1000);

    HookManager::initialize();

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



}
