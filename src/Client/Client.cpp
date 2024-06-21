#pragma once

#include "Client.hpp"
#include "GUI/Engine/Engine.hpp"
#include "../Utils/Versions/VersionUtils.hpp"
#include "../Utils/Versions/WinrtUtils.hpp"
#include <filesystem>
#include <thread>
#include <wingdi.h>
#include <wininet.h>

//winrt stuff
#include "winrt/windows.applicationmodel.core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/windows.system.h"


std::string Client::settingspath = Utils::getRoamingPath() + R"(\Flarial\Config\main.flarial)";
Settings Client::settings = Settings();

void DownloadAndSave(const std::string& url, const std::string& path) {

    if (Client::settings.getSettingByName<bool>("dlassets")->value || !std::filesystem::exists(path)) {
        char test[256];
        strcpy_s(test, "https://cdn-c6f.pages.dev/");
        if (InternetCheckConnectionA(test, FLAG_ICC_FORCE_CONNECTION, 0))
            URLDownloadToFileW(nullptr, FlarialGUI::to_wide(url).c_str(), FlarialGUI::to_wide(path).c_str(), 0, nullptr);
    }

}

bool Client::disable = false;

void setWindowTitle(std::wstring title) {
    using namespace winrt::Windows::UI::Notifications;
    using namespace winrt::Windows::UI::ViewManagement;
    using namespace winrt::Windows::ApplicationModel::Core;

    CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title = std::move(title)]() {
        ApplicationView::GetForCurrentView().Title(title);
        });
}

void Client::initialize() {
    setWindowTitle(L"Flarial");
    Logger::debug("[INIT] Initializing Flarial...");
    
    VersionUtils::init();
    Client::version = WinrtUtils::getFormattedVersion();
    Logger::debug("[INIT] Version: " + WinrtUtils::getVersion());
    Logger::debug("[INIT] Formatted: " + Client::version);

    if (!VersionUtils::isSupported(Client::version)) {
        Logger::debug("[INFO] Version not supported!");
        Client::disable = true;
        return;
    }

    VersionUtils::addData();

    std::filesystem::path folder_path(Utils::getRoamingPath() + "\\Flarial");
    if (!exists(folder_path)) {
        create_directory(folder_path);
    }

    std::filesystem::path folder_path2(Utils::getRoamingPath() + "\\Flarial\\assets");
    if (!exists(folder_path2)) {
        create_directory(folder_path2);
    }

    std::filesystem::path folder_path3(Utils::getRoamingPath() + "\\Flarial\\logs");
    if (!exists(folder_path3)) {
        create_directory(folder_path3);
    }

    std::filesystem::path folder_path4(Utils::getRoamingPath() + "\\Flarial\\Config");
    if (!exists(folder_path4)) {
        create_directory(folder_path4);
    }

    std::string Path = Utils::getRoamingPath() + R"(\Flarial\assets\)";

    std::string cdn_link = "https://flarialbackup.ashank.tech/";

    std::pair<std::string, std::string> fileData[] = {
            {cdn_link + "assets/gear.png",                  Path + "gear.png"},
            {cdn_link + "assets/font.ttf",                  Path + "font.ttf"},
            {cdn_link + "assets/logo.png",                  Path + "logo.png"},
            {cdn_link + "assets/flareal-dev.png",           Path + "flarial-dev.png"},
            {cdn_link + "assets/flarial-contribiutor.png",  Path + "flarial-contribiutor.png"},
            {cdn_link + "assets/flarial-premium.png",       Path + "flarial-premium.png"},
            {cdn_link + "assets/flarial-staff.png",         Path + "flarial-staff.png"},
            {cdn_link + "assets/re-q.png",                  Path + "re-q.png"},
            {cdn_link + "assets/fps.png",                   Path + "fps.png"},
            {cdn_link + "assets/like.png",                  Path + "like.png"},
            {cdn_link + "assets/auto_sprint.png",           Path + "auto_sprint.png"},
            {cdn_link + "assets/clickgui.png",              Path + "clickgui.png"},
            {cdn_link + "assets/modules.png",               Path + "modules.png"},
            {cdn_link + "assets/cursor.png",                Path + "cursor.png"},
            {cdn_link + "assets/magnify.png",               Path + "magnify.png"},
            {cdn_link + "assets/memory.png",                Path + "memory.png"},
            {cdn_link + "assets/time.png",                  Path + "time.png"},
            {cdn_link + "assets/fullbright.png",            Path + "fullbright.png"},
            {cdn_link + "assets/frying-pan.png",            Path + "frying-pan.png"},
            {cdn_link + "assets/font_bold.ttf",             Path + "font_bold.ttf"},
            {cdn_link + "assets/flarial-title.png",         Path + "flarial-title.png"},
            {cdn_link + "assets/combo.png",                 Path + "combo.png"},
            {cdn_link + "assets/reach.png",                 Path + "reach.png"},
            {cdn_link + "assets/keyboard.png",              Path + "keyboard.png"},
            {cdn_link + "assets/server-ip.png",             Path + "server-ip.png"},
            {cdn_link + "assets/coordinates.png",           Path + "coordinates.png"},
            {cdn_link + "assets/speed.png",                 Path + "speed.png"},
            {cdn_link + "assets/ping.png",                  Path + "ping.png"},
            {cdn_link + "assets/arrow.png",                 Path + "arrow.png"},
            {cdn_link + "assets/potion.png",                Path + "potion.png"},
            {cdn_link + "assets/nametag.png",               Path + "nametag.png"},
            {cdn_link + "assets/minecraftia.ttf",           Path + "minecraftia.ttf"},
            {cdn_link + "assets/blur.png",                  Path + "blur.png"},
            {cdn_link + "assets/eye.png",                   Path + "eye.png"},
            {cdn_link + "assets/chestplate.png",            Path + "chestplate.png"},
            {cdn_link + "assets/patar.jpg",                 Path + "patar.jpg"},
            {cdn_link + "assets/skull.png",                 Path + "skull.png"},
            {cdn_link + "assets/freelook.png",              Path + "freelook.png"},
            {cdn_link + "assets/hurt.png",                  Path + "hurt.png"},
            {cdn_link + "assets/smoke.png",                 Path + "smoke.png"},
            {cdn_link + "assets/renderoptions.png",         Path + "renderoptions.png"},
            {cdn_link + "assets/man.png",                   Path + "man.png"},
            {cdn_link + "assets/search.png",                Path + "search.png"},
            {cdn_link + "assets/slowly.png",                Path + "slowly.png"},
            {cdn_link + "assets/cloudy.png",                Path + "cloudy.png"},
            {cdn_link + "assets/list.png",                  Path + "list.png"},
            {cdn_link + "assets/text-box.png",              Path + "text-box.png"},
            {cdn_link + "assets/icognito.png",              Path + "icognito.png"},
            {cdn_link + "assets/stop.png",                  Path + "stop.png"},
            {cdn_link + "assets/block_break_indicator.png", Path + "block_break_indicator.png"},
            {cdn_link + "assets/field-of-view.png",         Path + "field-of-view.png"},
            {cdn_link + "assets/upside-down.png",           Path + "upside-down.png"},
            {cdn_link + "assets/down.png",                  Path + "down.png"},
            {cdn_link + "assets/Animations.png",            Path + "Animations.png"},
            {cdn_link + "assets/transparent.png",           Path + "transparent.png"},
            {cdn_link + "assets/rgb.png",                   Path + "rgb.png"},
            {cdn_link + "assets/pencil.png",                Path + "pencil.png"},
            {cdn_link + "assets/dvdlogo-01.png",            Path + "dvdlogo-01.png"},
            {cdn_link + "assets/dvdlogo-02.png",            Path + "dvdlogo-02.png"},
            {cdn_link + "assets/dvdlogo-03.png",            Path + "dvdlogo-03.png"},
            {cdn_link + "assets/dvdlogo-04.png",            Path + "dvdlogo-04.png"},
            {cdn_link + "assets/dvdlogo-05.png",            Path + "dvdlogo-05.png"},
            {cdn_link + "assets/dvdlogo-06.png",            Path + "dvdlogo-06.png"},
            {cdn_link + "assets/dvdlogo-07.png",            Path + "dvdlogo-07.png"},
            {cdn_link + "assets/dvdlogo-08.png",            Path + "dvdlogo-08.png"},
            {cdn_link + "assets/block.png",                 Path + "block.png"}
    };

    Client::CheckSettingsFile();
    Client::LoadSettings();

    if (Client::settings.getSettingByName<std::string>("fontname") == nullptr)
        Client::settings.addSetting("fontname", (std::string) "Space Grotesk");

    if (Client::settings.getSettingByName<std::string>("mod_fontname") == nullptr)
        Client::settings.addSetting("mod_fontname", (std::string) "Space Grotesk");

    if (Client::settings.getSettingByName<float>("blurintensity") == nullptr)
        Client::settings.addSetting("blurintensity", 18.0f);

    if (Client::settings.getSettingByName<bool>("killdx") == nullptr)
        Client::settings.addSetting("killdx", false);

    if (Client::settings.getSettingByName<bool>("disable_alias") == nullptr)
        Client::settings.addSetting("disable_alias", false);

    if (Client::settings.getSettingByName<bool>("vsync") == nullptr)
        Client::settings.addSetting("vsync", false);

    if (Client::settings.getSettingByName<bool>("disableanims") == nullptr)
        Client::settings.addSetting("disableanims", false);

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

    if (Client::settings.getSettingByName<std::string>("aliasingMode") == nullptr)
        Client::settings.addSetting("aliasingMode", (std::string) "Default");

    if (Client::settings.getSettingByName<std::string>("ejectKeybind") == nullptr)
        Client::settings.addSetting("ejectKeybind", (std::string) "");

    if (Client::settings.getSettingByName<bool>("enabledModulesOnTop") == nullptr)
        Client::settings.addSetting("enabledModulesOnTop", false);

    if (Client::settings.getSettingByName<float>("rgb_speed") == nullptr)
        Client::settings.addSetting("rgb_speed", 1.0f);

    if (Client::settings.getSettingByName<float>("rgb_saturation") == nullptr)
        Client::settings.addSetting("rgb_saturation", 1.0f);

    if (Client::settings.getSettingByName<float>("rgb_value") == nullptr)
        Client::settings.addSetting("rgb_value", 1.0f);

    // Create threads to download the files
    std::vector<std::thread> threads;
    for (const auto &data: fileData) {
        threads.emplace_back(DownloadAndSave, data.first, data.second);
    }

    // Wait for all threads to finish
    for (std::thread &thread: threads) {
        thread.join();
    }

    std::string fontpath = Utils::getRoamingPath() + R"(\Flarial\assets\font.ttf)";
    AddFontResource(fontpath.c_str());

    fontpath = Utils::getRoamingPath() + R"(\Flarial\assets\font_bold.ttf)";
    AddFontResource(fontpath.c_str());

    fontpath = Utils::getRoamingPath() + R"(\Flarial\assets\minecraftia.ttf)";
    AddFontResource(fontpath.c_str());

    Logger::initialize();

    HookManager::initialize();
    ModuleManager::initialize();
    Logger::debug("[Client] Ready.");

    if (!Client::disable) {
        FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "keybind")->value + " to open the menu in-game.");
    }
}

std::string window = "Minecraft";

HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = {0};
RECT clientRect = {0};
RECT previousRect = {0};
bool InHudScreen = false;

bool toes = false;

void Client::centerCursor() {
    if (hWnd != nullptr && Client::settings.getSettingByName<bool>("centreCursor")->value) {
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
                }
                if (Client::disable && !Client::settings.getSettingByName<bool>("centreCursor")->value) toes = false;
            });

            wow.detach();
        }


        if ((SDK::currentScreen != "hud_screen" && InHudScreen) ||
            (SDK::currentScreen == "hud_screen" && !InHudScreen)) {
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