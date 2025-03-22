#pragma once

#include <json/json.hpp>

class APIUtils {
  public:
  static std::vector<std::string> onlineUsers;
  static std::map<std::string, std::string> onlineVips;

  static std::pair<long, std::string> POST_Simple(const std::string &url, const std::string &postData);

  static std::pair<long, std::string> POST(const std::string &url, const std::string &postData, const std::string &contentType = "application/x-www-form-urlencoded", const std::vector<std::string> &
                                                   additionalHeaders = {});

  static std::pair<long, std::string> Request(const std::string& url, const std::string& method, const std::string& data = "", struct curl_slist* headers = nullptr);

  static std::string legacyGet(const std::string &URL);

  static std::string get(const std::string& link);
  static nlohmann::json getVips();
  static nlohmann::json getUsers();
  static bool hasRole(const std::string& role, const std::string& name);

  static std::vector<std::string> ListToVector(const std::string &listStr);

  static std::string VectorToList(const std::vector<std::string> &vec);

  static std::vector<std::string> UpdateVector(const std::vector<std::string> &currentVec,
                                               const std::vector<std::string> &commands);

  static std::vector<std::string> UpdateVector(const std::vector<std::string> &currentVec,
                                               const std::string &commandListStr);

  static std::vector<std::string> UpdateVectorFast(const std::vector<std::string> &currentVec,
                                                   const std::vector<std::string> &commands);
};