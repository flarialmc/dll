#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <json/json.hpp>

class Telemetry {
public:
    static void sendModuleEvent(const std::string& moduleName, const std::string& action);
    static std::string generateUserHash();
    static std::string getClientVersion();

private:
    static std::string getSystemIdentifier();
    static std::string sha256(const std::string& input);
    static void sendTelemetryAsync(const nlohmann::json& payload);
    static std::string s_userHash;
    static std::once_flag s_initFlag;
};