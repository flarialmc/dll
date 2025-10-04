#pragma once

#include <Command/Command.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include "../../../SDK/Client/Actor/LocalPlayer.hpp"

#define SERVER_IP "150.136.39.248"
#define SERVER_PORT 5010
#define BUFFER_SIZE 1024

class IRCChat : public Command {

    void listen_for_messages(SOCKET sock) {
        char buffer[BUFFER_SIZE];
        while (true) {
            int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
            if (bytesReceived <= 0) {
                std::cout << "Server disconnected" << std::endl;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::string received(buffer);
            size_t delimiterPos = received.find('|');
            if (delimiterPos != std::string::npos) {
                std::string recv_username = received.substr(0, delimiterPos);
                std::string recv_message = received.substr(delimiterPos + 1);
                if (SDK::clientInstance->getLocalPlayer()) {
                    SDK::clientInstance->getGuiData()->displayClientMessage("[IRC] "+recv_username + ": " + recv_message);
                }
                std::cout << recv_username << ": " << recv_message << std::endl;
            }
            else {
                std::cout << "Received invalid message format" << std::endl;
            }
        }
    }

    WSADATA wsaData;
    SOCKET clientSocket = INVALID_SOCKET;

    std::thread listener_thread;

public:
    IRCChat() : Command("irc", "Chat With other Flarial sigmas") {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOG_ERROR("WSAStartup failed: {}", WSAGetLastError());
            return;
        }

        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            LOG_ERROR("Socket creation failed: {}", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Server address setup
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

        // Connect to server
        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            LOG_ERROR("Connection failed: {}", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return;
        }

        listener_thread =  std::thread(&IRCChat::listen_for_messages, this, clientSocket);

    }
    void execute(const std::vector<std::string>& args) override;
};