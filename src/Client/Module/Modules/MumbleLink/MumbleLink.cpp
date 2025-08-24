#include "MumbleLink.hpp"

MumbleLink::MumbleLink(): Module("Mumble Link", "Use proximity chat in Flarial with the help of Mumble", IDR_MUMBLE_PNG, "")
{
    
}

void MumbleLink::onEnable()
{
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
                Context = getOps<std::string>("context").empty() ? SDK::getServerIP() : getOps<std::string>("context");
            }
            else {

            }

            Sleep(20);
        }
    });

    serverthread.detach();
    datathread.detach();

}

void MumbleLink::onDisable()
{
    Module::onDisable();
}

void MumbleLink::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("context", (std::string)"");
    
}

void MumbleLink::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Mumble Link");
    addTextBox("Channel/Room", "Keep empty to use the Server IP.", getOps<std::string>("context"));

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void MumbleLink::SendDataToServer()
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
    while (ModuleManager::getModule("Mumble Link")->getOps<bool>("enabled")) {
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
