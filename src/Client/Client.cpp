#include "Client.hpp"

#include <filesystem>
#include <thread>
#include <wingdi.h>
#include <wininet.h>

#include <Utils/VersionUtils.hpp>
#include <Utils/WinrtUtils.hpp>

#include "Command/CommandManager.hpp"

Settings Client::settings = Settings();

bool notifiedOfConnectionIssue = false;

std::string Client::current_commit = COMMIT_HASH;

std::vector<std::string> Client::getPlayersVector(const nlohmann::json& data) {
    std::vector<std::string> allPlayers;

    try {
        // Iterate through key-value pairs in the JSON object
        for (const auto& [key, value] : data.items()) {
            if (value.contains("players") && value.at("players").is_array()) {
                for (const auto& player : value.at("players")) {
                    if (player.is_string()) {
                        allPlayers.push_back(player.get<std::string>());
                    }
                }
            }
        }
    } catch (const nlohmann::json::exception& e) {
        Logger::error("Error parsing players: {}", e.what());
    }

    if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {
        try {
            std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();
            std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));

            if (clearedName.empty()) {
                clearedName = String::removeColorCodes(name);
            }
            allPlayers.push_back(clearedName);
        } catch (const std::exception& e) {
            Logger::error("Error processing local player name: {}", e.what());
        }
    }

    return allPlayers;
}

bool Client::disable = false;


void Client::initialize() {
    winrt::init_apartment();

#if defined(__TEST__)
    WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {} {}", FLARIAL_VERSION, FLARIAL_BUILD_TYPE, FLARIAL_BUILD_DATE));
#else
    WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {}", FLARIAL_VERSION, FLARIAL_BUILD_TYPE));
#endif

    VersionUtils::initialize();
    version = VersionUtils::getFormattedVersion();

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
        Utils::getRoamingPath() + "\\Flarial\\Config",
        Utils::getRoamingPath() + "\\Flarial\\scripts",
    };

    for (const auto& path : directories) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directory(path);
        }
    }

    Client::CheckSettingsFile();
    Client::LoadSettings();

    ADD_SETTING("fontname", std::string("Space Grotesk"));
    ADD_SETTING("mod_fontname", std::string("Space Grotesk"));
    ADD_SETTING("blurintensity", 2.0f);
    ADD_SETTING("killdx", false);
    ADD_SETTING("disable_alias", false);
    ADD_SETTING("vsync", false);
    ADD_SETTING("promotions", true);
    ADD_SETTING("donotwait", true);
    ADD_SETTING("bufferingmode", std::string("Double Buffering"));
    ADD_SETTING("swapeffect", std::string("FLIP_SEQUENTIAL"));
    ADD_SETTING("disableanims", false);
    ADD_SETTING("anonymousApi", false);
    ADD_SETTING("dlassets", true);
    ADD_SETTING("noicons", false);
    ADD_SETTING("noshadows", false);
    ADD_SETTING("watermark", true);
    ADD_SETTING("centreCursor", false);
    ADD_SETTING("aliasingMode", std::string("Default"));
    ADD_SETTING("ejectKeybind", std::string(""));
    ADD_SETTING("enabledModulesOnTop", false);
    ADD_SETTING("rgb_speed", 1.0f);
    ADD_SETTING("rgb_saturation", 1.0f);
    ADD_SETTING("rgb_value", 1.0f);
    ADD_SETTING("modules_font_scale", 1.0f);
    ADD_SETTING("gui_font_scale", 1.0f);
    ADD_SETTING("overrideFontWeight", false);
    ADD_SETTING("fontWeight", std::string("Normal"));

    FlarialGUI::ExtractImageResource(IDR_RED_LOGO_PNG, "red-logo.png","PNG");
    FlarialGUI::ExtractImageResource(IDR_CYAN_LOGO_PNG, "dev-logo.png", "PNG");
    FlarialGUI::ExtractImageResource(IDR_GAMER_LOGO_PNG, "gamer-logo.png", "PNG");
    FlarialGUI::ExtractImageResource(IDR_BOOSTER_LOGO_PNG, "booster-logo.png", "PNG");

    FlarialGUI::LoadFont(IDR_FONT_TTF);

    FlarialGUI::LoadFont(IDR_FONT_BOLD_TTF);

    FlarialGUI::LoadFont(IDR_MINECRAFTIA_TTF);

    HookManager::initialize();
    ModuleManager::initialize();
    CommandManager::initialize();
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