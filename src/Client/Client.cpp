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
        { "https://cdn-c6f.pages.dev/assets/like.png", Path + "like.png" },
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
        { "https://cdn-c6f.pages.dev/assets/speed.png", Path + "speed.png" },
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
        { "https://cdn-c6f.pages.dev/assets/list.png", Path + "list.png" },
        { "https://cdn-c6f.pages.dev/assets/text-box.png", Path + "text-box.png" },
        { "https://cdn-c6f.pages.dev/assets/icognito.png", Path + "icognito.png" },
        { "https://cdn-c6f.pages.dev/assets/stop.png", Path + "stop.png" },
        { "https://cdn-c6f.pages.dev/assets/block_break_indicator.png", Path + "block_break_indicator.png" },
        { "https://cdn-c6f.pages.dev/assets/field-of-view.png", Path + "field-of-view.png" },
        { "https://cdn-c6f.pages.dev/assets/upside-down.png", Path + "upside-down.png" },
        { "https://cdn-c6f.pages.dev/assets/down.png", Path + "down.png" },
        { "https://cdn-c6f.pages.dev/assets/Animations.png", Path + "Animations.png" }
    };

    Client::CheckSettingsFile();
    Client::LoadSettings();

    if (Client::settings.getSettingByName<std::string>("fontname") == nullptr)
        Client::settings.addSetting("fontname", (std::string)"Space Grotesk");

    if (Client::settings.getSettingByName<float>("blurintensity") == nullptr)
        Client::settings.addSetting("blurintensity", 18.0f);

    if (Client::settings.getSettingByName<bool>("killdx") == nullptr)
        Client::settings.addSetting("killdx", false);

    if (Client::settings.getSettingByName<bool>("vsync") == nullptr)
        Client::settings.addSetting("vsync", false);

    if (Client::settings.getSettingByName<bool>("anonymousApi") == nullptr)
        Client::settings.addSetting("anonymousApi", false);

    if (Client::settings.getSettingByName<bool>("dlassets") == nullptr)
        Client::settings.addSetting("dlassets", true);

    if (Client::settings.getSettingByName<bool>("noicons") == nullptr)
        Client::settings.addSetting("noicons", false);

    if (Client::settings.getSettingByName<bool>("noshadows") == nullptr)
        Client::settings.addSetting("noshadows", false);

    if (Client::settings.getSettingByName<bool>("watermark") == nullptr)
        Client::settings.addSetting("watermark", true);

    if (Client::settings.getSettingByName<bool>("centreCursor") == nullptr)
        Client::settings.addSetting("centreCursor", false);

    if (Client::settings.getSettingByName<std::string>("ejectKeybind") == nullptr)
        Client::settings.addSetting("ejectKeybind", (std::string)"");

    if (Client::settings.getSettingByName<bool>("enabledModulesOnTop") == nullptr)
        Client::settings.addSetting("enabledModulesOnTop", true);

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

    if (!Client::disable) {
        FlarialGUI::Notify("Report bugs at https://flarial.net/discord!");
        FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("keybind")->value + " to open the menu in-game.");
    }
}

std::string window = "Minecraft";

HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = { 0 };
RECT clientRect = { 0 };
RECT previousRect = { 0 };
bool InHudScreen = false;

bool toes = false;

void Client::centerCursor() {
    if (hWnd != NULL && Client::settings.getSettingByName<bool>("centreCursor")->value) {
        if (!toes) {
            toes = true;
            std::thread wow([&]() {
                while (!Client::disable && Client::settings.getSettingByName<bool>("centreCursor")->value) {
                    GetWindowRect(hWnd, &currentRect);
                    GetClientRect(hWnd, &clientRect);

                    if (memcmp(&currentRect, &previousRect, sizeof(RECT)) != 0) {
                        previousRect = currentRect;
                    }
                    Sleep(1000);
                };
                if (Client::disable && !Client::settings.getSettingByName<bool>("centreCursor")->value) toes = false;
                });

            wow.detach();
        }

        
        if ((SDK::CurrentScreen != "hud_screen" && InHudScreen) || (SDK::CurrentScreen == "hud_screen" && !InHudScreen))  {
            GetWindowRect(hWnd, &currentRect);
            GetClientRect(hWnd, &clientRect);

            int windowX = currentRect.left;
            int windowY = currentRect.top;

            int centerX = windowX + (clientRect.right) / 2;
            int centerY = windowY + (clientRect.bottom) / 2;

            SetCursorPos(centerX, centerY);

            InHudScreen = !InHudScreen;
        }
    }
}