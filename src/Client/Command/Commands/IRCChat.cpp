#include "IRCChat.hpp"

#include "SDK/Client/Actor/LocalPlayer.hpp"

void IRCChat::execute(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        addCommandMessage("You cannot send an empty message.");
        return;
    }
    std::ostringstream oss;
    for (size_t i = 0; i < args.size(); ++i) {
        oss << args[i];
        oss << " ";
    }

    std::string output;
    for (char c : oss.str()) {
        if (c == ' ') {
            output += " ";
        }
        else {
            output += c;
        }
    }

    std::string username = SDK::clientInstance->getLocalPlayer()->getPlayerName();

    std::string to_send = username + "|" + output;
    if (send(clientSocket, to_send.c_str(), to_send.length(), 0) == SOCKET_ERROR) {
        addCommandMessage("Send failed: {}", WSAGetLastError());
    }
    else {
        SDK::clientInstance->getGuiData()->displayClientMessage("[IRC] " + username + ": " + output);
    }
}
