#include <filesystem>
#include <fstream>
#include "ActorBaseTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "RaknetTick.hpp"
#include "../../../Client.hpp"

bool ActorBaseTick::allahuakbar = false;

void ActorBaseTick::enableHook() {


    auto vft = Memory::findSig("48 8D 05 ? ? ? ? 48 89 01 B8 ? ? ? ? 8D 50 FA 44 8D 48 ? 44 8D 40 ? 66 89 44 ? ? E8 ? ? ? ? 48 8B 8B"); // Player vtable
    int xd = *reinterpret_cast<int*>(vft + 3);
    uintptr_t** vftREAL = reinterpret_cast<uintptr_t**>(vft + xd + 7);

    this->manualHook(vftREAL[30], callback, (void**)&xed);


}

ActorBaseTick::ActorBaseTick() : Hook("ActorBaseTick", "48 8D 05 ???? ? ? 48 89 01 B8 ? ? ? ? 8D 50 FA 44 8D 48 ? 44 8D 40 ? 66 89 44 ? ? E8 ? ? ? ? 48 8B 8B"){}

void ActorBaseTick::callback(Actor *xd) {

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


