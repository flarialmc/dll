#pragma once

// Made by marioCST, ported by FreezeEngine

#include <string>
#include <unordered_map>
#include <functional>

class VersionUtils {
public:
    static void init();
    static bool isSupported(const std::string& version);
    static void addData();

    static std::vector<std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>>> versions;
};
