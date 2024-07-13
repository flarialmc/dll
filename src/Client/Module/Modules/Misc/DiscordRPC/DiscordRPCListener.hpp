#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class DiscordRPCListener : public Listener {
public:
    std::string previousIp;

    void onTick(TickEvent &event) override {
        handleServerIpChange();
    }

    void onRender(RenderEvent &event) override {
        if (SDK::getServerPing() == -1) {
            handleServerIpChange();
        }
    }

public:
    explicit DiscordRPCListener(const char string[5]) {
        this->name = string;
    }

private:
    void handleServerIpChange() {
        std::string ip = SDK::getServerIP();

        if (ip != previousIp) {
            static std::string settingspath = Utils::getRoamingPath() + "\\Flarial\\serverip.txt";

            if (!std::filesystem::exists(settingspath)) {
                createSettingsFile(settingspath);
            }
            updateSettingsFile(settingspath, ip);
        }
    }

    void createSettingsFile(const std::string &settingspath) {
        std::filesystem::path filePath(settingspath);
        std::filesystem::create_directories(filePath.parent_path());

        HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (fileHandle == INVALID_HANDLE_VALUE) {
            Logger::error("Failed to create file: " + settingspath);
            return;
        }

        CloseHandle(fileHandle);
    }

    void updateSettingsFile(const std::string &settingspath, const std::string &ip) {
        std::ofstream outputFile(settingspath);
        if (outputFile.is_open()) {
            outputFile << ip;
            previousIp = ip;
            outputFile.close();
        } else {
            Logger::error("Failed to open file: " + settingspath);
        }
    }
};
