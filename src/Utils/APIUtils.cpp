#include "APIUtils.hpp"

#include <wininet.h>
#include <Utils/Utils.hpp>
#include <Utils/Logger/Logger.hpp>

std::vector<std::string> APIUtils::onlineUsers;
std::map<std::string, std::string> APIUtils::onlineVips;

std::string APIUtils::get(const std::string &link) {
    try {
        HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!interwebs) {
            return "";
        }

        std::string rtn;
        HINTERNET urlFile = InternetOpenUrlA(interwebs, link.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (urlFile) {
            char buffer[2000];
            DWORD bytesRead;
            while (InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                rtn.append(buffer, bytesRead);
            }
            InternetCloseHandle(urlFile);
        }

        InternetCloseHandle(interwebs);
        return String::replaceAll(rtn, "|n", "\r\n");
    } catch (const std::exception &e) {
        Logger::error(e.what());
    }
    return "";
}

nlohmann::json APIUtils::getVips() {
    try {
        std::string users = get("https://api.flarial.xyz/vips");

        if (users.empty()) {
            Logger::warn("Unable to fetch vips, API is down or you are not connected to the internet.");
            return nlohmann::json::object();
        }

        if (nlohmann::json::accept(users)) {
            return nlohmann::json::parse(users);
        }

        Logger::warn("VIP JSON rejected: {}", users);
        return nlohmann::json::object();
    }
    catch (const nlohmann::json::parse_error& e) {
        Logger::error("An error occurred while parsing vip users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        Logger::error("An unexpected error occurred: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json APIUtils::getUsers() {
    try {
        std::string users = get("https://api.flarial.xyz/servers");

        if (users.empty()) {
            Logger::warn("Unable to fetch users, API is down or you are not connected to the internet.");
            return nlohmann::json::object();
        }

        if (nlohmann::json::accept(users)) {
            return nlohmann::json::parse(users);
        }

        Logger::warn("Users JSON rejected: {}", users);
        return nlohmann::json::object();
    }
    catch (const nlohmann::json::parse_error& e) {
        Logger::error("An error occurred while parsing online users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        Logger::error("An unexpected error occurred: {}", e.what());
        return nlohmann::json::object();
    }
}

bool APIUtils::hasRole(const std::string& role, const std::string& name) {
    try {
        auto it = onlineVips.find(name);
        bool onlineVip = it != onlineVips.end() && it->second == role;
        bool online = std::find(onlineUsers.begin(), onlineUsers.end(), name) != onlineUsers.end();
        return onlineVip || (!onlineVip && online && role == "Regular");
    } catch (const std::exception& e) {
        Logger::error("An error occurred while checking roles: {}", e.what());
        return false;
    }
}