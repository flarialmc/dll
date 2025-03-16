#pragma once

#include <random>
#include <winrt/impl/windows.storage.2.h>
#include <wininet.h>
#include "../../../../Client.hpp"

class ScriptMarketplace : public Listener {

public:
    ScriptMarketplace() {
        Listen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
        Listen(this, ProtocolEvent, &ScriptMarketplace::onProtocolConfig);
    }

    ~ScriptMarketplace() {
        Deafen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
        Deafen(this, ProtocolEvent, &ScriptMarketplace::onProtocolConfig);
    }

    std::string scriptPath() {
        using namespace winrt::Windows::Storage;

        auto roamingFolder = Utils::getRoamingPath();
        std::filesystem::path scriptsDir = std::filesystem::path(roamingFolder) / "Flarial" / "scripts";

        return scriptsDir.string();
    }

    static std::string GetString(const std::string &URL) {
        try {
            HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (!interwebs) {
                return "";
            }

            std::string rtn;
            HINTERNET urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
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
        } catch (const std::exception &e) {
            Logger::error(e.what());
        }
        return "";
    }

    void onProtocol(ProtocolEvent event) {
        if (event.getPath() == std::wstring(L"flarial-scripting")) {
            for (const auto &pair: event.getProtocolArgs()) {
                if (pair.first == std::wstring(L"scriptName")) {
                    std::string id = String::WStrToStr(pair.second);
                    Logger::info("script id {}", id);
                    std::string url = "http://node2.sear.host:5019/api/scripts/" + id + "/download";
                    std::string data = GetString(url);
                    std::ofstream file(Utils::getClientPath() + "\\Scripts\\Modules\\tmpd.tmp", std::ios::binary);
                    Logger::info("data: {}", data.c_str());
                    file.write(data.c_str(), data.size());
                    file.close();

                    ModuleManager::restartModules = true;
                }
            }
        }
    }

    void onProtocolConfig(ProtocolEvent event) {
        if (event.getPath() == std::wstring(L"flarial-configs")) {
             for (const auto &pair: event.getProtocolArgs()) {
                if (pair.first == std::wstring(L"configName")) {
                    std::string id = String::WStrToStr(pair.second);
                    Logger::info("config name {}", id);
                    std::string url = "http://node2.sear.host:5019/api/configs/" + id + "/download";
                    std::string data = ScriptMarketplace::GetString(url);
                    std::ofstream file(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp", std::ios::binary);
                    Logger::info("data: {}", data.c_str());
                    file.write(data.c_str(), data.size());
                    file.close();

                    std::string configname = id;
                    Client::createConfig(configname);
                    std::string to = Utils::getRoamingPath() + "\\Flarial\\Config\\" + configname;

                    Utils::extractFromFile(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp", to);
                    std::filesystem::remove(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp");

                    FlarialGUI::Notify("Imported Config as: " + configname);

                    reloadAllConfigs();
                }
            }
        }
    }

    static void reloadAllConfigs() {
        Client::availableConfigs.clear();
        Client::loadAvailableConfigs();
        ModuleManager::restartModules = true;
    }
};
