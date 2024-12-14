#pragma once

#include <json/json.hpp>

class APIUtils {
  public:
  static std::vector<std::string> onlineUsers;
  static std::map<std::string, std::string> onlineVips;

  static std::string get(const std::string& link);
  static nlohmann::json getVips();
  static nlohmann::json getUsers();
  static bool hasRole(const std::string& role, const std::string& name);
};