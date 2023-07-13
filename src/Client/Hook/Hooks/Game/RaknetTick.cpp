#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include <filesystem>
#include <fstream>

 std::string RaknetTickHook::towriteip = "";

void RaknetTickHook::callback(RaknetConnector* raknet)  {

	SDK::raknetConnector = raknet;

    std::string ip = raknet->JoinedIp;

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

    if(SDK::clientInstance != nullptr)
    if(towriteip != ip && SDK::clientInstance->getLocalPlayer() != nullptr) {

        std::ofstream outputFile(settingspath);
        if (outputFile.is_open()) {
            outputFile << raknet->JoinedIp + " " + SDK::clientInstance->getLocalPlayer()->playerName;
            outputFile.close();
        }

        towriteip = raknet->JoinedIp;
    }
}



void RaknetTickHook::enableHook() {
	this->autoHook(callback, (void**)raknetTickOriginal);
}
