#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

Vec3<float> Pos = Vec3<float>();
float yaw, pitch = 0;
std::string Context, PlayerName = "";

#include <iostream>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

void SendDataToServer()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create socket
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << std::endl;
        WSACleanup();
        return;
    }

    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to server
    if (connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    std::cout << "Connected to Mumble link!" << std::endl;

    // Data to send


    // Send data continuously
    while (ModuleManager::getModule("Mumble Link")->settings.getSettingByName<bool>("enabled")->value) {
        // Send floats
        if (send(ConnectSocket, (char*)&Pos.x, sizeof(Pos.x), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. Error: " << WSAGetLastError() << std::endl;
            break;
        }

        if (send(ConnectSocket, (char*)&Pos.y, sizeof(Pos.y), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. f2" << std::endl;
            break;
        }
        if (send(ConnectSocket, (char*)&Pos.z, sizeof(Pos.z), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. f3" << std::endl;
            break;
        }

        if (send(ConnectSocket, (char*)&yaw, sizeof(yaw), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. yaw" << std::endl;
            break;
        }
        if (send(ConnectSocket, (char*)&pitch, sizeof(pitch), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. pitch" << std::endl;
            break;
        }

        // Send first string length and data
        uint32_t len1 = PlayerName.length();
        if (send(ConnectSocket, (char*)&len1, sizeof(len1), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. s1" << std::endl;
            break;
        }
        if (send(ConnectSocket, PlayerName.c_str(), len1, 0) == SOCKET_ERROR) {
            std::cout << "Send failed. s11" << std::endl;
            break;
        }

        // Send second string length and data
        uint32_t len2 = Context.length();
        if (send(ConnectSocket, (char*)&len2, sizeof(len2), 0) == SOCKET_ERROR) {
            std::cout << "Send failed. s2" << std::endl;
            break;
        }
        if (send(ConnectSocket, Context.c_str(), len2, 0) == SOCKET_ERROR) {
            std::cout << "Send failed. s22" << std::endl;
            break;
        }

        Sleep(20);
    }

    // Close connection
    closesocket(ConnectSocket);
    WSACleanup();
}

class MumbleLink : public Module {
public:

    MumbleLink() : Module("Mumble Link", "Fixes Minecraft's default input delay", IDR_CURSOR_PNG, "") {
        Module::setup();

	void onEnable() override {
		Module::onEnable();
		std::thread serverthread([&]() {
            while (this->isEnabled()) {
                SendDataToServer();
                Sleep(5000);
            }
			});
		std::thread datathread([&]() {
			while (this->isEnabled()) {
				if (SDK::clientInstance and SDK::clientInstance->getLocalPlayer()) {
					Pos = *SDK::clientInstance->getLocalPlayer()->getPosition();
                    auto rot = SDK::clientInstance->getLocalPlayer()->getActorRotationComponent()->rot;
                    yaw = rot.x;
                    pitch = rot.y;
					PlayerName = SDK::clientInstance->getLocalPlayer()->getPlayerName();
					Context = this->settings.getSettingByName<std::string>("context")->value.empty() ? SDK::getServerIP() : this->settings.getSettingByName<std::string>("context")->value;
				}
                else {

                }

                Sleep(20);
            }
            });

        serverthread.detach();
        datathread.detach();

    };

	void onEnable() override {
		Module::onEnable();
	}

    void onDisable() override {
        Module::onDisable();
    }

	void defaultConfig() override {
		Module::defaultConfig();
		if (!this->settings.getSettingByName<std::string>("context")) {
			settings.addSetting<std::string>("context", (std::string)"");
		}
	}

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		this->addHeader("General");
		this->addTextBox("Channel/Room", "Keep empty to use the Server IP.", this->settings.getSettingByName<std::string>("context")->value);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};
