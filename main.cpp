#pragma once

#include <windows.h>
#include <algorithm>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
//#include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero.h>
#include <wininet.h>

#include "src/Client/Module/Modules/Nick/NickListener.hpp"

std::chrono::steady_clock::time_point lastBeatTime;

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace);

std::string DownloadString(std::string URL);

std::string removeColorCodes(const std::string& input);

DWORD WINAPI init(HMODULE real)
{
#ifndef NDEBUG
    bool shouldDebug = false; // Change this bool locally, NEVER push it set to true

    if (GetConsoleWindow() == nullptr && shouldDebug) {
        AllocConsole();
        SetConsoleTitleA("Flarial-Debugger");
        FILE *out;
        freopen_s(&out, ("CONOUT$"), ("w"), stdout);
    }
#endif

    Client::initialize();
    Logger::info("[Client] Initializing");

    std::thread statusThread([]() {
        while (true) {

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);

            if(!Client::disable) {
                if(SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                        if(elapsed >= std::chrono::seconds(60)) {
                            ModuleManager::onlineUsers.clear();
                            std::string name = SDK::clientInstance->getLocalPlayer()->playerName;
                            ModuleManager::onlineUsers.push_back(Utils::removeColorCodes(name));
                            std::string pp = DownloadString("https://api.flarial.synthetix.host/users");

                            json playersDict;

                            try {
                                playersDict = json::parse(pp);
                            } catch (const json::parse_error& e) {
                                Logger::error(e.what());
                                lastBeatTime = now;
                                continue;
                            }

                            int totalPlaytime = 0;
                            int numberOfPlayers = 0;

                            for (const auto& player : playersDict.items()) {
                                try {
                                    if (!player.value().contains("lastbeat") || !player.value()["lastbeat"].is_number()) {
                                        std::cerr << "Invalid or missing 'lastbeat' for player: " << player.key() << std::endl;
                                        continue;
                                    }

                                    std::string name2 = Utils::removeNonAlphanumeric(player.key());
                                    name2 = replaceAll(name2, "�", "");
                                    ModuleManager::onlineUsers.push_back(name2);
                                } catch (const std::exception& e) {
                                    std::cerr << "Error processing player: " << player.key() << " - " << e.what() << std::endl;
                                    continue;
                                }
                            }

                            std::string ipToSend = SDK::getServerIP();

                            if (!Client::settings.getSettingByName<bool>("anonymousApi")->value) {
                                if (ipToSend.find("none") != std::string::npos) ipToSend = "in.singleplayer";
                                else if (!ipToSend.empty());
                                else ipToSend = "in.singleplayer";
                            } else ipToSend = "is.anonymous";

                            auto module = ModuleManager::getModule("Nick");

                            if(SDK::clientInstance != nullptr)
                            if(SDK::clientInstance->getLocalPlayer() != nullptr)
                            if (module->isEnabled()) {
                                name = Utils::removeNonAlphanumeric(Utils::removeColorCodes(NickListener::original));
                                name = replaceAll(name, "�", "");

                            }
                            // send thing
                            std::cout << DownloadString(std::format("https://api.flarial.synthetix.host/heartbeat/{}/{}",Utils::removeColorCodes(name),ipToSend)) << std::endl;

                            lastBeatTime = now;
                        }
                    }
                }
                Sleep(60);

            } else break;
        }
    });
    statusThread.detach();


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

    Logger::debug("[Kiero] Shut down Kiero.");

    ResizeHook::cleanShit();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    Logger::debug("[MinHook] Freeing Library.");

    Sleep(100);

    FreeLibraryAndExitThread(real, 1);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Client::currentModule = instance;
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        ModuleManager::terminate();
    }

    return TRUE;
}

std::string DownloadString(std::string URL) {
    HINTERNET interwebs = InternetOpenA("holyshitplsdonthurtme", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
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