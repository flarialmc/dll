#pragma once

#include "Client.hpp"
#include "GUI/Engine/Engine.hpp"
#include "../Utils/Versions/VersionUtils.hpp"
#include <filesystem>
#include <thread>
#include <wingdi.h>
#include <wininet.h>

//winrt stuff
#include "winrt/windows.applicationmodel.core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/windows.system.h"

std::string Client::settingspath = Utils::getConfigsPath() + "\\main.flarial";
Settings Client::settings = Settings();
std::vector<std::string> Client::onlinePlayers;

bool notifiedOfConnectionIssue = false;

std::string Client::current_commit = COMMIT_HASH;

std::vector<std::string> Client::getPlayersVector(const nlohmann::json& data) {
    std::vector<std::string> allPlayers;

    for (const auto& it : data) {
        if (it.contains("players")) {
            for (const auto& player : it.at("players")) {
                allPlayers.push_back(player);
            }
        }
    }

    if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {
        std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();
        std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));

        if (clearedName.empty()) clearedName = String::removeColorCodes(name);
        allPlayers.push_back(clearedName);
    }

    return allPlayers;
}

bool Client::disable = false;

void Client::setWindowTitle(std::wstring title) {
    using namespace winrt::Windows::UI::ViewManagement;
    using namespace winrt::Windows::ApplicationModel::Core;

    CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title = std::move(title)]() {
        ApplicationView::GetForCurrentView().Title(title);
        });
}

void Client::initialize() {
    setWindowTitle(L"Flarial " + String::StrToWStr(WinrtUtils::getFormattedVersion() + " " + current_commit));

    VersionUtils::init();
    Client::version = WinrtUtils::getFormattedVersion();

    if (!VersionUtils::isSupported(Client::version)) {
        Logger::fatal("Minecraft version is not supported!");
        MessageBox(nullptr, "Flarial: this version is not supported!", "", MB_OK);
        ModuleManager::terminate();
        Client::disable = true;
        return;
    }

    VersionUtils::addData();

    std::vector<std::filesystem::path> directories = {
        Utils::getRoamingPath() + "\\Flarial",
        Utils::getRoamingPath() + "\\Flarial\\assets",
        Utils::getRoamingPath() + "\\Flarial\\logs",
        Utils::getRoamingPath() + "\\Flarial\\Config"
    };

    for (const auto& path : directories) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directory(path);
        }
    }

    Client::CheckSettingsFile();
    Client::LoadSettings();

    if (Client::settings.getSettingByName<std::string>("fontname") == nullptr)
        Client::settings.addSetting("fontname", (std::string) "Space Grotesk");

    if (Client::settings.getSettingByName<std::string>("mod_fontname") == nullptr)
        Client::settings.addSetting("mod_fontname", (std::string) "Space Grotesk");

    if (Client::settings.getSettingByName<float>("blurintensity") == nullptr)
        Client::settings.addSetting("blurintensity", 2.0f);

    if (Client::settings.getSettingByName<bool>("killdx") == nullptr)
        Client::settings.addSetting("killdx", false);

    if (Client::settings.getSettingByName<bool>("disable_alias") == nullptr)
        Client::settings.addSetting("disable_alias", false);

    if (Client::settings.getSettingByName<bool>("vsync") == nullptr)
        Client::settings.addSetting("vsync", false);

    if (Client::settings.getSettingByName<bool>("promotions") == nullptr)
        Client::settings.addSetting("promotions", true);

    if (Client::settings.getSettingByName<bool>("donotwait") == nullptr)
        Client::settings.addSetting("donotwait", true);

    if (Client::settings.getSettingByName<std::string>("bufferingmode") == nullptr)
        Client::settings.addSetting("bufferingmode", (std::string) "Double Buffering");

    if (Client::settings.getSettingByName<std::string>("swapeffect") == nullptr)
        Client::settings.addSetting("swapeffect", (std::string) "FLIP_SEQUENTIAL");


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

    if (Client::settings.getSettingByName<float>("modules_font_scale") == nullptr)
        Client::settings.addSetting("modules_font_scale", 1.0f);

    if (Client::settings.getSettingByName<float>("gui_font_scale") == nullptr)
        Client::settings.addSetting("gui_font_scale", 1.0f);

    if (Client::settings.getSettingByName<bool>("overrideFontWeight") == nullptr)
        Client::settings.addSetting("overrideFontWeight", (bool)false);

    if (Client::settings.getSettingByName<std::string>("fontWeight") == nullptr)
        Client::settings.addSetting("fontWeight", (std::string) "Normal");

    if (Client::settings.getSettingByName<bool>("fralier") == nullptr)
        Client::settings.addSetting("fralier", (bool)false);

    FlarialGUI::ExtractImageResource(IDR_RED_LOGO_PNG, "red-logo.png","PNG");

    FlarialGUI::LoadFont(IDR_FONT_TTF);

    FlarialGUI::LoadFont(IDR_FONT_BOLD_TTF);

    FlarialGUI::LoadFont(IDR_MINECRAFTIA_TTF);

    HookManager::initialize();
    ModuleManager::initialize();
}

std::string window = "Minecraft";

HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = {0};
RECT clientRect = {0};
RECT previousRect = {0};
bool InHudScreen = false;

bool toes = false;

void Client::centerCursor() {
    if(Client::disable) return;
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


        if ((SDK::getCurrentScreen() != "hud_screen" && InHudScreen) ||
            (SDK::getCurrentScreen() == "hud_screen" && !InHudScreen)) {
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