#pragma once

#include <windows.h>
#include <algorithm>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
#include "src/Client/Hook/Hooks/Game/RaknetTick.hpp"
#include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero.h>
#include <wininet.h>


std::chrono::steady_clock::time_point lastBeatTime;

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace);

std::string DownloadString(std::string URL);

std::string removeColorCodes(const std::string& input);

DWORD WINAPI init(HMODULE real)
{


    bool showConsole = true;
    
    if (GetConsoleWindow() == nullptr && showConsole) {
        AllocConsole();
        SetConsoleTitleA("Flarial-Debugger");
        FILE *out;
        freopen_s(&out, ("CONOUT$"), ("w"), stdout);
    }
    



    Client::initialize();
    Logger::info("Initializing Client");
    /* API unavalible, crashes
    std::thread statusThread([]() {
        while (true) {

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);

            if(!Client::disable) {
                if(SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                        if(elapsed >= std::chrono::seconds(60)) {
                            ModuleManager::OnlineUsers.clear();
                            std::string name = SDK::clientInstance->getLocalPlayer()->playerName;
                            ModuleManager::OnlineUsers.push_back(Utils::removeColorCodes(name));
                            std::string pp = DownloadString("https://api.flarial.net/users");

                            json playersDict = json::parse(pp);

                            int totalPlaytime = 0;
                            int numberOfPlayers = 0;

                            for (const auto& player : playersDict.items()) {

                                std::time_t unixTimestamp = player.value()["lastbeat"];
                                std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(unixTimestamp);
 
                                ModuleManager::OnlineUsers.push_back(Utils::removeNonAlphanumeric(player.key()));
                               
                                //std::cout << Utils::removeNonAlphanumeric(player.key()) << std::endl;
                      
                            }

                            std::string ipToSend;

                            if (!Client::settings.getSettingByName<bool>("anonymousApi")->value) {
                                if (RaknetTickHook::towriteip.find("none") != std::string::npos) ipToSend = "in.singleplayer";
                                else if (!RaknetTickHook::towriteip.empty()) ipToSend = RaknetTickHook::towriteip;
                                else ipToSend = "in.singleplayer";
                            } else ipToSend = "is.anonymous";

                            auto module = ModuleManager::getModule("Nick");

                            if(SDK::clientInstance != nullptr)
                            if(SDK::clientInstance->getLocalPlayer() != nullptr)
                            if (module->settings.getSettingByName<bool>("enabled")->value) {
                                name = Utils::removeNonAlphanumeric(Utils::removeColorCodes(NickListener::original));
                            }
                            // send thing
                            std::cout << DownloadString(std::format("https://api.flarial.net/heartbeat/{}/{}",Utils::removeColorCodes(name),ipToSend))

                            + " " + std::format("https://api.flarial.net/heartbeat/{}/{}",
                              Utils::removeColorCodes(name),
                                ipToSend) << std::endl;

                            lastBeatTime = now;
                        }
                    }
                }
                Sleep(60);

            } else break;
        }
    });
    statusThread.detach();
    */
    while (true) {
        if (Client::disable) {
            break;
        } else {

            Sleep(50);
        }
    }

    Client::SaveSettings();

    Logger::info("Uninitializing Client");

    EventHandler::unregisterAll();

    ModuleManager::terminate();
    HookManager::terminate();

    kiero::shutdown();

    Logger::debug("Shut down Kiero.");

    ResizeHook::CleanShit();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    Logger::debug("Freeing Library.");

    Sleep(100);

    FreeLibraryAndExitThread(real, 1);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        ModuleManager::terminate();
    }

    return TRUE;
}

std::string DownloadString(std::string URL) {
    HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    HINTERNET urlFile;
    std::string rtn;
    if (interwebs) {
        urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
        if (urlFile) {
            char buffer[2000];
            DWORD bytesRead;
            do {
                InternetReadFile(urlFile, buffer, 2000, &bytesRead);
                rtn.append(buffer, bytesRead);
                memset(buffer, 0, 2000);
            } while (bytesRead);
            InternetCloseHandle(interwebs);
            InternetCloseHandle(urlFile);
            std::string p = replaceAll(rtn, "|n", "\r\n");
            return p;
        }
    }
    InternetCloseHandle(interwebs);
    std::string p = replaceAll(rtn, "|n", "\r\n");
    return p;
}

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

std::string removeColorCodes(const std::string& input) {
    std::string result;
    bool skipNext = false;

    for (char c : input) {
        if (skipNext) {
            skipNext = false;
        } else if (c == L'§') {
            skipNext = true;
        } else {
            result += c;
        }
    }

    return result;
}