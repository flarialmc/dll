#include <windows.h>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventManager.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
#include "src/Client/Module/Modules/ClickGUI/ClickGUI.hpp"
// #include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero/kiero.h>
#include <wininet.h>
#include <Utils/WinrtUtils.hpp>
#include <Utils/Audio.hpp>

#include "curl/curl/curl.h"
#include "Scripting/EventManager/ScriptingEventManager.hpp"
#include "src/Utils/Logger/crashlogs.hpp"
#include "src/Client/Module/Modules/Nick/NickModule.hpp"
#include "src/Client/Command/CommandManager.hpp"
#include "src/SDK/Client/Options/OptionsParser.hpp"
#include "Utils/APIUtils.hpp"

std::chrono::steady_clock::time_point lastBeatTime;
std::chrono::steady_clock::time_point lastVipFetchTime;
std::chrono::steady_clock::time_point lastOnlineUsersFetchTime;
std::chrono::steady_clock::time_point lastAnnouncementTime;
static HANDLE mutex;


void SavePlayerCache() {
    std::string playersListString = APIUtils::VectorToList(APIUtils::onlineUsers);

    std::string filePath = Utils::getRoamingPath() + "/Flarial/playerscache.txt";
    std::ofstream cacheFile(filePath);
    if (cacheFile.is_open()) {
        cacheFile << playersListString;
        cacheFile.close();
        Logger::success("Cached player list.");
    } else {
        Logger::error("Could not open file for writing: " + filePath);
    }
}

float Client::elapsed;
uint64_t Client::start;

DWORD WINAPI init() {
    Client::start = Utils::getCurrentMs();
    Logger::initialize();
    std::thread lol([]() { Audio::init(); });
    lol.detach(); // ???
    Client::initialize();

    Client::elapsed = (Utils::getCurrentMs() - Client::start) / 1000.0;

    Logger::success("Flarial initialized in {:.2f}s", Client::elapsed);

    OptionsParser parser;
    parser.parseOptionsFile();

    std::thread statusThread([]() {
    while (!Client::disable) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);
        auto onlineUsersFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastOnlineUsersFetchTime);
        auto onlineAnnouncementElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastAnnouncementTime);
        auto vipFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastVipFetchTime);

        if (!SDK::hasInstanced || SDK::clientInstance == nullptr || SDK::clientInstance->getLocalPlayer() == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            continue;
        }

        if (elapsed >= std::chrono::seconds(60)) {
            std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();
            std::string ipToSend = SDK::getServerIP();

            if (Client::settings.getSettingByName<bool>("anonymousApi")->value) {
                ipToSend = "is.anonymous";
            } else if (ipToSend.find("none") != std::string::npos || ipToSend.empty()) {
                ipToSend = "in.singleplayer";
            }

            auto module = ModuleManager::getModule("Nick");
            if (module && module->isEnabled()) {
                name = String::removeNonAlphanumeric(String::removeColorCodes(NickModule::original));
                name = String::replaceAll(name, "�", "");
            }

            std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
            if (clearedName.empty()) clearedName = String::removeColorCodes(name);

            if (clearedName != "skinStandardCust") {
                APIUtils::legacyGet(std::format("https://api.flarial.xyz/heartbeat/{}/{}", clearedName, ipToSend));
                lastBeatTime = now;
            }
        }

        if (onlineUsersFetchElapsed >= std::chrono::minutes(3)) {
            try {
                std::string data = APIUtils::VectorToList(APIUtils::onlineUsers);
                std::pair<long, std::string> post = APIUtils::POST_Simple("https://api.flarial.xyz/allOnlineUsers", data);
                APIUtils::onlineUsers = APIUtils::UpdateVector(APIUtils::onlineUsers, post.second);
                SavePlayerCache();
                lastOnlineUsersFetchTime = now;
            } catch (const std::exception &ex) {
                Logger::error("An error occurred while parsing online users: {}", ex.what());
            }
        }

        if (vipFetchElapsed >= std::chrono::minutes(3)) {
            try {
                auto vipsJson = APIUtils::getVips();
                std::map<std::string, std::string> updatedVips;

                for (const auto& [role, users] : vipsJson.items()) {
                    if (users.is_array()) {
                        for (const auto& user : users) {
                            if (user.is_string()) {
                                updatedVips[user.get<std::string>()] = role;
                            }
                        }
                    }
                }

                if (!updatedVips.empty()) {
                    APIUtils::onlineVips = std::move(updatedVips);
                }
                lastVipFetchTime = now;
            } catch (const std::exception& e) {
                Logger::error("An error occurred while parsing VIP users: {}", e.what());
            }
        }

        if (onlineAnnouncementElapsed >= std::chrono::minutes(10) && ModuleManager::initialized &&
            Client::settings.getSettingByName<bool>("promotions")->value) {
            SDK::clientInstance->getGuiData()->displayClientMessage(
                "§khiii §r §n§l§4FLARIAL §r§khiii \n§r§cDonate to Flarial! §ehttps://flarial.xyz/donate\n§9Join our discord! §ehttps://flarial.xyz/discord"
            ); 
            lastAnnouncementTime = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
});

    statusThread.detach();

    while (!Client::disable) {
        ModuleManager::syncState();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Client::SaveSettings();

    Client::UnregisterActivationHandler();
    ModuleManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "ModuleManager", "Shut down");
    HookManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "HookManager", "Shut down");
    CommandManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "CommandManager", "Shut down");

    ResizeHook::cleanShit();
    Logger::custom(fmt::fg(fmt::color::pink), "DirectX", "Cleaning");

    kiero::shutdown();

    Logger::custom(fmt::fg(fmt::color::pink), "Kiero", "Shut down");

    Scripting::unloadModules();
    ScriptingEventManager::clearHandlers();

    Audio::cleanup();

    Logger::custom(fmt::fg(fmt::color::pink), "Audio", "Shut down");


    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    glaiel::crashlogs::end_session();

    Logger::custom(fmt::fg(fmt::color::cadet_blue), "MinHook", "Freed Library");

    WinrtUtils::setWindowTitle("");

    Logger::shutdown();

    CloseHandle(mutex);
    FreeLibraryAndExitThread(Client::currentModule, 0);
}


BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        /*
        https://learn.microsoft.com/en-us/windows/uwp/communication/sharing-named-objects
        Ensure a single instance of Flarial Client is loaded.
        Launchers may use this mutex to detect if the client is injected or not.
        */
        mutex = CreateMutexW(NULL, FALSE, L"Flarial");
        if (GetLastError()) {
            CloseHandle(mutex);
            return FALSE;
        }

        DisableThreadLibraryCalls(instance);
        Client::currentModule = instance;
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr));
    }

    return TRUE;
}