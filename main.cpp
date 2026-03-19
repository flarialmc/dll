#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "src/Client/Client.hpp"
#include "src/Client/Hook/Manager.hpp"
#include "src/Client/Events/EventManager.hpp"
#include "src/Client/Hook/Hooks/Render/DirectX/DXGI/ResizeHook.hpp"
#include "src/Client/Module/Modules/ClickGUI/ClickGUI.hpp"
// #include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero/kiero.h>
#include <wininet.h>
#include <Utils/WinrtUtils.hpp>
#include <Utils/Audio.hpp>

#include "curl/curl/curl.h"
#include "Hook/Hooks/Game/UnicodeWndProcHack.hpp"
#include "Modules/Misc/DiscordRPC/DiscordRPCListener.hpp"
#include "Scripting/ScriptManager.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "src/Utils/Logger/crashlogs.hpp"
#include "src/Client/Module/Modules/Nick/NickModule.hpp"
#include "src/Client/Command/CommandManager.hpp"
#include "src/SDK/Client/Options/OptionsParser.hpp"
#include "Utils/APIUtils.hpp"
#include "Utils/Threading.hpp"
#include "Utils/BeaconWindow.hpp"

#include <condition_variable>
#include <mutex>

std::chrono::steady_clock::time_point lastBeatTime;
std::chrono::steady_clock::time_point lastVipFetchTime;
std::chrono::steady_clock::time_point lastOnlineUsersFetchTime;
std::chrono::steady_clock::time_point lastAnnouncementTime;
static HANDLE mutex;

// SEH wrapper to safely get local player during shutdown
static LocalPlayer* safeGetLocalPlayer(ClientInstance* inst) {
    __try {
        return inst->getLocalPlayer();
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

static std::mutex statusThreadMutex;
static std::condition_variable statusThreadCV;
static std::thread statusThread;


static std::string urlEncode(const std::string& value) {
    std::string out;
    out.reserve(value.size() * 3);
    const char* hex = "0123456789ABCDEF";

    for (unsigned char c : value) {
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else {
            out.push_back('%');
            out.push_back(hex[(c >> 4) & 0xF]);
            out.push_back(hex[c & 0xF]);
        }
    }

    return out;
}

static std::string tryResolveLocalXuidFromState() { 
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return "";

    auto* player = SDK::clientInstance->getLocalPlayer();
    auto* level = player->getLevel();
    if (!level) return "";

    std::string localName = String::removeNonAlphanumeric(String::removeColorCodes(player->getPlayerName()));
    if (localName.empty()) localName = String::removeColorCodes(player->getPlayerName());
    localName = String::toLower(localName);

    for (const auto& pair : level->getPlayerMap()) {
        std::string entryName = String::removeNonAlphanumeric(String::removeColorCodes(pair.second.name));
        if (entryName.empty()) entryName = String::removeColorCodes(pair.second.name);
        if (String::toLower(entryName) == localName && !pair.second.XUID.empty()) {
            return pair.second.XUID;
        }
    }
    return "";
}

void SavePlayerCache() {
    std::string playersListString = APIUtils::VectorToList(APIUtils::onlineUsers);

    // Check if string size is greater than 15 KB (15,360 bytes)
    if (playersListString.size() > 15360) {
        playersListString = "{\"players\":[]}";
    }

    std::string filePath = Utils::getRoamingPath() + "/Flarial/playerscache.txt";
    std::ofstream cacheFile(filePath);
    if (cacheFile.is_open()) {
        cacheFile << playersListString;
        cacheFile.close();
        Logger::success("Cached player list.");
    } else {
        LOG_ERROR("Could not open file for writing: " + filePath);
    }
}
// hi
float Client::elapsed;
uint64_t Client::start;

DWORD WINAPI init() {
    Client::start = Utils::getCurrentMs();
    Logger::initialize();
    std::thread lol([]() { Audio::init(); });
    lol.detach();
    Client::initialize();

    BeaconWindow::create();

    Client::elapsed = (Utils::getCurrentMs() - Client::start) / 1000.0;

    Logger::debug("Thread ID: {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    Logger::success("Flarial initialized in {:.2f}s", Client::elapsed);

    OptionsParser parser;
    parser.parseOptionsFile();

    statusThread = std::thread([]() {
        while (!Client::disable) {
            try {
                // Sleep at the start to give shutdown a chance to complete
                // Use condition variable so we can wake up immediately on shutdown
                {
                    std::unique_lock<std::mutex> lock(statusThreadMutex);
                    if (statusThreadCV.wait_for(lock, std::chrono::milliseconds(60),
                                                [] { return Client::disable; })) {
                        break; // Shutdown requested
                    }
                }

                if (Client::disable) break;

                // Capture pointer locally and validate before any virtual calls
                if (!SDK::hasInstanced) continue;
                auto *clientInst = SDK::clientInstance;
                if (!clientInst || Client::disable) continue;

                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);
                auto onlineUsersFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(
                        now - lastOnlineUsersFetchTime);
                auto onlineAnnouncementElapsed = std::chrono::duration_cast<std::chrono::seconds>(
                        now - lastAnnouncementTime);
                auto vipFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastVipFetchTime);

                if (Client::disable) break;

                auto& playerState = Client::getPlayerState();

                if (!playerState.isInWorld() || Client::disable) continue;

                static std::string xuid = tryResolveLocalXuidFromState();

                if (xuid == "") {
                    xuid = tryResolveLocalXuidFromState();
                }

                if ((elapsed >= std::chrono::seconds(60)) and xuid != "") {

                    std::string name = playerState.getUserName();
                    std::string ipToSend = playerState.getServerIP();

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

                        std::string heartbeatUrl = std::format("https://api.flarial.xyz/heartbeat/{}/{}", urlEncode(clearedName), urlEncode(ipToSend));
                        if (!xuid.empty()) heartbeatUrl += "?xuid=" + urlEncode(xuid);

                        APIUtils::legacyGet(heartbeatUrl);
                        lastBeatTime = now;
                    }
                }

                if (onlineUsersFetchElapsed >= std::chrono::minutes(3) &&
                    Client::settings.getSettingByName<bool>("apiusage")->value) {
                    try {
                        std::string data = APIUtils::VectorToList(APIUtils::onlineUsers);
                        std::pair<long, std::string> post = APIUtils::POST_Simple(
                                "https://api.flarial.xyz/allOnlineUsers", data);
                        APIUtils::onlineUsers = APIUtils::UpdateVector(APIUtils::onlineUsers, post.second, playerState.getUserName());
                        APIUtils::onlineUsersSet =
                                APIUtils::onlineUsers | std::ranges::to<decltype(APIUtils::onlineUsersSet)>();
                        SavePlayerCache();
                        lastOnlineUsersFetchTime = now;
                    } catch (const std::exception &ex) {
                        LOG_ERROR("An error occurred while parsing online users: {}", ex.what());
                    }
                }

                if (vipFetchElapsed >= std::chrono::minutes(3) &&
                    Client::settings.getSettingByName<bool>("apiusage")->value) {
                    try {
                        auto vipsJson = APIUtils::getVips();
                        decltype(APIUtils::vipUserToRole) updatedVips;

                        for (const auto &[role, users]: vipsJson.items()) {
                            if (users.is_array()) {
                                for (const auto &user: users) {
                                    if (user.is_string()) {
                                        updatedVips[user.get<std::string>()] = role;
                                    }
                                }
                            }
                        }

                        if (!updatedVips.empty()) {
                            APIUtils::vipUserToRole = std::move(updatedVips);
                        }
                        lastVipFetchTime = now;
                    } catch (const std::exception &e) {
                        LOG_ERROR("An error occurred while parsing VIP users: {}", e.what());
                    }
                }

                if (onlineAnnouncementElapsed >= std::chrono::minutes(10) && ModuleManager::initialized &&
                    Client::settings.getSettingByName<bool>("promotions")->value) {
                    if (SDK::clientInstance and SDK::clientInstance->getGuiData()) {
                        SDK::clientInstance->getGuiData()->displayClientMessage(
                                "§khiii §r §n§l§4FLARIAL §r§khiii \n§r§cDonate to Flarial! §ehttps://flarial.xyz/donate\n§9Join our discord! §ehttps://flarial.xyz/discord"
                        );
                        lastAnnouncementTime = now;
                    }
                }

            } catch (...) {
                // Catch any exception during shutdown to prevent crash
                if (Client::disable) break;
            }
        }
    });


    while (!Client::disable) {
        ThreadingUtils::getInstance().executeMainThreadQueue();
        ModuleManager::syncState();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Signal the status thread to wake up and exit
    statusThreadCV.notify_all();

    // Wait for the status thread to finish before proceeding with cleanup
    if (statusThread.joinable()) {
        statusThread.join();
    }
    Logger::custom(fmt::fg(fmt::color::pink), "StatusThread", "Shut down");

    ModuleManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "ModuleManager", "Shut down");

    Client::UnregisterActivationHandler();
    Logger::custom(fmt::fg(fmt::color::pink), "UnregisterActivationHandler", "Shut down");
    ScriptManager::shutdown();
    Logger::custom(fmt::fg(fmt::color::pink), "ScriptManager", "Shut down");

    HookManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "HookManager", "Shut down");
    CommandManager::terminate();
    Logger::custom(fmt::fg(fmt::color::pink), "CommandManager", "Shut down");

    ResizeHook::cleanupResources();
    Logger::custom(fmt::fg(fmt::color::pink), "DirectX", "Cleaning");

    kiero::shutdown();

    Logger::custom(fmt::fg(fmt::color::pink), "Kiero", "Shut down");

    Audio::cleanup();

    Logger::custom(fmt::fg(fmt::color::pink), "Audio", "Shut down");


    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    glaiel::crashlogs::end_session();

    Logger::custom(fmt::fg(fmt::color::cadet_blue), "MinHook", "Freed Library");

    WinrtUtils::setWindowTitle("");

    BeaconWindow::destroy();
    Logger::custom(fmt::fg(fmt::color::pink), "BeaconWindow", "Shut down");

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
