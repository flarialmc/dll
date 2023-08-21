#include <filesystem>
#include <fstream>
#include "ActorNormalTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "RaknetTick.hpp"
#include "../../../Client.hpp"

bool ActorNormalTick::allahuakbar = false;

void ActorNormalTick::enableHook() {


    auto vft = Memory::findSig("48 8D 05 ?? ?? ?? ?? 48 89 01 49 8B 00 48 89 41 08");
    int xd = *reinterpret_cast<int*>(vft + 3);
    uintptr_t** vftREAL = reinterpret_cast<uintptr_t**>(vft + xd + 7);

    this->manualHook(vftREAL[44], callback, (void**)&xed);


}

ActorNormalTick::ActorNormalTick() : Hook("ActorNormalTickHook", "48 8D 05 ???? ?? ?? 48 89 01 49 88 00 48 89 41 08"){}

void ActorNormalTick::callback(Actor *xd) {

    if (Client::disable) return;

    if(xd != nullptr) {

        if(!allahuakbar && RaknetTickHook::towriteip.find("none") != std::string::npos) {

            allahuakbar = true;

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


                std::ofstream outputFile(settingspath);

                if (outputFile.is_open()) {
                    outputFile << "world";
                    outputFile.close();
                }
        }


        xed(xd);
        TickEvent event(xd);
        EventHandler::onTick(event);
    }

    

}


