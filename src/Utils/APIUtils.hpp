#pragma once

#include <json/json.hpp>

template<typename T>
struct transparent_hash : std::hash<T> { using is_transparent = void; };

class APIUtils {
  public:
  static inline std::vector<std::string> onlineUsers{};
  static inline std::unordered_set<std::string, transparent_hash<std::string_view>, std::equal_to<>> onlineUsersSet{};
  static inline std::map<std::string, std::string, std::less<>> vipUserToRole{};

  static std::pair<long, std::string> POST_Simple(const std::string &url, const std::string &postData);

  static std::pair<long, std::string> POST(const std::string &url, const std::string &postData, const std::string &contentType = "application/x-www-form-urlencoded", const std::vector<std::string> &
                                                   additionalHeaders = {});

  static std::pair<long, std::string> Request(const std::string& url, const std::string& method, const std::string& data = "", struct curl_slist* headers = nullptr);

  static std::string legacyGet(const std::string &URL);

  static std::string get(const std::string& link);
  static nlohmann::json getVips();
  static nlohmann::json getUsers();
  static bool hasRole(std::string_view role, std::string_view name);

  static std::vector<std::string> ListToVector(const std::string &listStr);

  static std::string VectorToList(const std::vector<std::string> &vec);

  static std::vector<std::string> UpdateVector(const std::vector<std::string> &currentVec,
                                               const std::vector<std::string> &commands);

  static std::vector<std::string> UpdateVector(const std::vector<std::string> &currentVec,
                                               const std::string &commandListStr);

  static std::vector<std::string> UpdateVectorFast(const std::vector<std::string> &currentVec,
                                                   const std::vector<std::string> &commands);
};