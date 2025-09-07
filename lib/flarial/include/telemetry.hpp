#pragma once

#include <string>

class telemetry {
public:
    static void sendModuleEvent(const std::string& moduleName, const std::string& action);
    static std::string generateUserHash();
    static std::string getClientVersion();
};