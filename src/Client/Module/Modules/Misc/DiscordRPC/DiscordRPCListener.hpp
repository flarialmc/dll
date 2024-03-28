#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class DiscordRPCListener : public Listener {

public:

    void onRaknetTick(RaknetTickEvent &event) override {
        static bool once = false;

        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (!once) {
                    once = true;
                }

                std::string ip = SDK::getServerIP();

                if (ip.empty() && SDK::clientInstance == nullptr) {
                    ip = "none";
                }

                if (ip.empty() && SDK::clientInstance != nullptr) {

                    if (SDK::clientInstance->getLocalPlayer() == nullptr) {
                        ip = "none";
                    }
                }

                std::string settingspath = Utils::getRoamingPath() + "\\Flarial\\serverip.txt";

                if (!std::filesystem::exists(settingspath)) {

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

                if (SDK::getServerIP() != ip) {

                    std::ofstream outputFile(settingspath);
                    if (outputFile.is_open()) {
                        outputFile << ip;
                        outputFile.close();
                    }

                }
            }
        }

    }

    void onTick(TickEvent &event) override { // TODO: writes every tick
        if(SDK::getServerIP() == "world" || SDK::getServerIP() == "none") {
            std::string settingspath = Utils::getRoamingPath() + "\\Flarial\\serverip.txt";

            if (!std::filesystem::exists(settingspath)) {

                std::filesystem::path filePath(settingspath);
                std::filesystem::create_directories(filePath.parent_path());

                HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ,
                                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                if (fileHandle == INVALID_HANDLE_VALUE) {
                    Logger::error("Failed to create file: " + settingspath);
                    return;
                }

                CloseHandle(fileHandle);
            }


            std::ofstream outputFile(settingspath);

            if (outputFile.is_open()) {
                outputFile << SDK::getServerIP();
                outputFile.close();
            }
        }
    }

public:
    explicit DiscordRPCListener(const char string[5]) {
        this->name = string;
    }
};
