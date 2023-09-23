#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include "ActorNormalTick.hpp"
#include <filesystem>
#include <fstream>

std::string RaknetTickHook::towriteip = "";

void RaknetTickHook::callback(RaknetConnector* raknet)  {

    static bool  once = false;

    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            if (!once) {
                Logger::debug(std::format("nigger {}", (void*)raknet));
                once = true;
            }

            SDK::raknetConnector = raknet;

            std::string ip = raknet->JoinedIp;

            if (ip.empty() && SDK::clientInstance == nullptr) {
                ip = "none";
            }

            if (ip.empty() && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() == nullptr) {
                    ip = "none";
                    ActorNormalTick::allahuakbar = false;
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

            if (towriteip != ip) {

                std::ofstream outputFile(settingspath);
                if (outputFile.is_open()) {
                    outputFile << ip;
                    outputFile.close();
                }

                towriteip = ip;
            }
        }
    }

    raknetTickOriginal(raknet);
}



void RaknetTickHook::enableHook() {
    this->autoHook(callback, (void**)&raknetTickOriginal);
}