#pragma once

#include <windows.h>
#include <algorithm>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventManager.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
//#include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero.h>
#include <wininet.h>

#include "src/Utils/Logger/crashlogs.hpp"
#include "src/Client/Module/Modules/Nick/NickModule.hpp"

std::chrono::steady_clock::time_point lastBeatTime;
std::chrono::steady_clock::time_point lastOnlineUsersFetchTime;
std::chrono::steady_clock::time_point lastAnnouncementTime;

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace);

std::string DownloadString(std::string URL);

std::string removeColorCodes(const std::string& input);

void printVector(const std::vector<std::string>& vec) {
    for (const auto& str : vec) {
        std::cout << str << std::endl;
    }
}


DWORD WINAPI init(HMODULE real) {
    Logger::initialize();

    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (!Utils::isMinecraftLoaded(hProcess)) {
        Logger::info("Waiting for Minecraft to load...");

        while (!Utils::isMinecraftLoaded(hProcess)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    CloseHandle(hProcess);

    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    Logger::info("Waiting for Minecraft to load...");
    while (!Utils::isMinecraftLoaded(hProcess)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    Logger::info("Minecraft loaded!");

    CloseHandle(hProcess);

    Client::initialize();

    std::thread statusThread([]() {
        while (!Client::disable) {

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);
            auto onlineUsersFetchElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastOnlineUsersFetchTime);
            auto onlineAnnouncementElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastAnnouncementTime);


            if(!Client::disable) {
                if(SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                        if(elapsed >= std::chrono::seconds(60)) {
                            std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();

                            std::string ipToSend = SDK::getServerIP();

                            if (!Client::settings.getSettingByName<bool>("anonymousApi")->value) {
                                if (ipToSend.find("none") != std::string::npos) ipToSend = "in.singleplayer";
                                else if (!ipToSend.empty());
                                else ipToSend = "in.singleplayer";
                            } else ipToSend = "is.anonymous";

                            auto module = ModuleManager::getModule("Nick");

                            if(SDK::clientInstance != nullptr)
                            if(SDK::clientInstance->getLocalPlayer() != nullptr) {
                                if (module && module->isEnabled()) {
                                    name = Utils::removeNonAlphanumeric(
                                            Utils::removeColorCodes(NickModule::original));
                                    name = replaceAll(name, "�", "");

                                }
                                std::string clearedName = Utils::removeNonAlphanumeric(Utils::removeColorCodes(name));
                                if (clearedName.empty()) clearedName = Utils::removeColorCodes(name);
                                if(clearedName == "skinStandardCust") return;
                                // send thing
                                DownloadString(
                                        std::format("https://api.flarial.synthetix.host/heartbeat/{}/{}", clearedName,
                                                    ipToSend));

                                lastBeatTime = now;
                            }
                        }
                        if(onlineUsersFetchElapsed >= std::chrono::minutes(3)) {

                            auto onlineUsersRaw = DownloadString("https://api.flarial.synthetix.host/servers");

                            nlohmann::json playersDict;

                            try {
                                playersDict = nlohmann::json::parse(onlineUsersRaw);
                                Client::onlinePlayers = Client::getPlayersVector(playersDict);
                            }  catch (const json::parse_error& e) {
                                Logger::error("An error occurred while parsing online users: {}", e.what());
                                lastOnlineUsersFetchTime = now;
                                continue;
                            }

                            lastOnlineUsersFetchTime = now;
                        }

                        if(ModuleManager::initialized) {
                            if(SDK::clientInstance && onlineAnnouncementElapsed >= std::chrono::minutes(10)) {
                                if(Client::settings.getSettingByName<bool>("promotions")->value) SDK::clientInstance->getGuiData()->displayClientMessage("§khiii §r §n§l§4FLARIAL §r§khiii \n§r§cDonate to Flarial! §ehttps://flarial.xyz/donate\n§9Join our discord! §ehttps://flarial.xyz/discord");
                                lastAnnouncementTime = now;
                            }
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
        }

        ModuleManager::syncState();
    }

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


BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(instance);
            Client::currentModule = instance;
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) init, instance, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            break;
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