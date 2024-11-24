#include <windows.h>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventManager.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
// #include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero.h>
#include <wininet.h>

#include "src/Utils/Logger/crashlogs.hpp"
#include "src/Client/Module/Modules/Nick/NickModule.hpp"

std::chrono::steady_clock::time_point lastBeatTime;
std::chrono::steady_clock::time_point lastOnlineUsersFetchTime;
std::chrono::steady_clock::time_point lastAnnouncementTime;

std::string DownloadString(const std::string& url);

DWORD WINAPI init() {

    uint64_t start = Utils::getCurrentMs();
    Logger::initialize();

    Client::initialize();

    float elapsed = (Utils::getCurrentMs() - start) / 1000.0;


    Logger::success("Flarial initialized in {:.2f}s", elapsed);

    //Logger::printColors();
    std::thread statusThread([]() {
    while (!Client::disable) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);
        auto onlineUsersFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastOnlineUsersFetchTime);
        auto onlineAnnouncementElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastAnnouncementTime);

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
                DownloadString(std::format("https://api.flarial.synthetix.host/heartbeat/{}/{}", clearedName, ipToSend));
                lastBeatTime = now;
            }
        }

        if (onlineUsersFetchElapsed >= std::chrono::minutes(3)) {
            //fetch online users
            try {
                std::string onlineUsersRaw = Utils::downloadFile("https://api.flarial.synthetix.host/servers");
                Client::onlinePlayers = Client::getPlayersVector(nlohmann::json::parse(onlineUsersRaw));
                lastOnlineUsersFetchTime = now;
            } catch (const nlohmann::json::parse_error &e) {
                Logger::error("An error occurred while parsing online users: {}", e.what());
            }
            //fetch online vips
            try {
                //std::string onlineVipsRaw = Utils::downloadFile("https://api.flarial.synthetix.host/vips"); //get online Vips through api
                
                //static string for testing only until api endpoint is done
                //feel free to enter your ign and try it out!
                //"treegfx",
                
                std::string onlineVipsRaw = R"({
                    "Dev": [
                        "FreezeEngine",
                        "EpiclyRasp26",
                        "TapeClientMC",
                        "marioCST",
                        "Withor2301",
                        "ANSHUL MASTER",
                        "StoneHunter2020",
                        "treegfx"
                    ],
                    "Gamer": [
                        "Gamer"
                    ],
                    "Booster": [
                        "Booster"
                    ]
                })";
                Client::onlineVips = nlohmann::json::parse(onlineVipsRaw);
                lastOnlineUsersFetchTime = now;
            }
            catch (const nlohmann::json::parse_error& e) {
                Logger::error("An error occurred while parsing online vips: {}", e.what());
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

    std::thread syncThread([]() {
        while (!Client::disable) {
            ModuleManager::syncState();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    syncThread.detach();

    while(!Client::disable) std::this_thread::sleep_for(std::chrono::milliseconds(10));


    Client::SaveSettings();

    ResizeHook::cleanShit();

    kiero::shutdown();

    Logger::custom(fmt::fg(fmt::color::pink), "Kiero", "Shut down");

    ModuleManager::terminate();
    HookManager::terminate();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    glaiel::crashlogs::end_session();

    Logger::custom(fmt::fg(fmt::color::cadet_blue), "MinHook", "Freed Library");

    Client::setWindowTitle(L"");

    Logger::shutdown();

    FreeLibraryAndExitThread(Client::currentModule, 0);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        /*
        https://learn.microsoft.com/en-us/windows/uwp/communication/sharing-named-objects
        Ensure a single instance of Flarial Client is loaded.
        Launchers may use this mutex to detect if the client is injected or not.
        */
        HANDLE hMutex = CreateMutexW(NULL, FALSE, L"Flarial");
        if (GetLastError())
        {
            CloseHandle(hMutex);
            return FALSE;
        }

        DisableThreadLibraryCalls(instance);
        Client::currentModule = instance;
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr));
    }

    return TRUE;
}

std::string DownloadString(const std::string& url) {
    HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!interwebs) {
        return "";
    }

    std::string rtn;
    HINTERNET urlFile = InternetOpenUrlA(interwebs, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (urlFile) {
        char buffer[2000];
        DWORD bytesRead;
        while (InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            rtn.append(buffer, bytesRead);
        }
        InternetCloseHandle(urlFile);
    }

    InternetCloseHandle(interwebs);
    return String::replaceAll(rtn, "|n", "\r\n");
}

void fetchVips() {
    auto now = std::chrono::steady_clock::now();
}
