#pragma once

#include <map>
#include <string>
#include <vector>

class api {
public:
    static std::vector<std::string> onlineUsers;
    static std::map<std::string, std::string> onlineVips;

    static nlohmann::json fetchVips();
    static nlohmann::json fetchUsers();
    static bool hasRole(const std::string& role, const std::string& name);

    static std::vector<std::string> listToVector(const std::string &listStr);
    static std::string vectorToList(const std::string& level, const std::vector<std::string>& vec);

    static bool updateVector(const std::string& name);
    static bool heartbeat(const std::string& name, const std::string& level);
};