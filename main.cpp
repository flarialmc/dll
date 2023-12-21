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

using namespace std;

chrono::steady_clock::time_point lastBeatTime;

string replaceAll(string subject, const string& search,
                       const string& replace);

string DownloadString(string URL);

string removeColorCodes(const string& input);

DWORD WINAPI init(HMODULE real)
{


    bool showConsole = false;
    
    if (GetConsoleWindow() == nullptr && showConsole) {
        AllocConsole();
        SetConsoleTitleA("Flarial-Debugger");
        FILE *out;
        freopen_s(&out, ("CONOUT$"), ("w"), stdout);
    }
    



    Client::initialize();
    Logger::info("Initializing Client");

    thread statusThread([]() {
        while (true) {

            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - lastBeatTime);

            if(!Client::disable) {
                if(SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                        if(elapsed >= chrono::seconds(60)) {
                            string ipToSend;

                            if (!Client::settings.getSettingByName<bool>("anonymousApi")->value) {
                                if (RaknetTickHook::towriteip.find("none") != string::npos) ipToSend = "in.singleplayer";
                                else if (!RaknetTickHook::towriteip.empty()) ipToSend = RaknetTickHook::towriteip;
                                else ipToSend = "in.singleplayer";
                            } else ipToSend = "is.anonymous";

                            string name = SDK::clientInstance->getLocalPlayer()->playerName;

                            auto module = ModuleManager::getModule("Nick");

                            if(SDK::clientInstance != nullptr)
                            if(SDK::clientInstance->getLocalPlayer() != nullptr)
                            if (module->settings.getSettingByName<bool>("enabled")->value) {
                                name = Utils::removeNonAlphanumeric(Utils::removeColorCodes(NickListener::original));
                            }
                            // send thing
                            cout << DownloadString(format("https://api.flarial.net/heartbeat/{}/{}",Utils::removeColorCodes(name),ipToSend))

                            + " " + format("https://api.flarial.net/heartbeat/{}/{}",
                              Utils::removeColorCodes(name),
                                ipToSend) << endl;
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

string DownloadString(string URL) {
    HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    HINTERNET urlFile;
    string rtn;
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
            string p = replaceAll(rtn, "|n", "\r\n");
            return p;
        }
    }
    InternetCloseHandle(interwebs);
    string p = replaceAll(rtn, "|n", "\r\n");
    return p;
}

string replaceAll(string subject, const string& search,
                       const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

string removeColorCodes(const string& input) {
    string result;
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
