#pragma once

#include <string>
#include <stacktrace>
#include <json/json_fwd.hpp>
#include <windows.h>

class CrashTelemetry {
public:
    // Initialize crash telemetry system
    static void initialize();

    // Send crash telemetry data
    static void sendCrashReport(
        const std::stacktrace& trace,
        int signal = 0,
        const std::string& signalName = "",
        EXCEPTION_POINTERS* exceptionPointers = nullptr
    );

    // Generate system information
    static nlohmann::json getSystemInfo();

    // Generate hardware information
    static nlohmann::json getHardwareInfo();

    // Generate client information
    static nlohmann::json getClientInfo();

    // Generate session information
    static nlohmann::json getSessionInfo();

    // Generate module states
    static nlohmann::json getModuleStates();

    // Generate crash information
    static nlohmann::json getCrashInfo(
        const std::stacktrace& trace,
        int signal,
        const std::string& signalName,
        EXCEPTION_POINTERS* exceptionPointers = nullptr
    );

    // Get exception details from EXCEPTION_POINTERS
    static nlohmann::json getExceptionDetails(EXCEPTION_POINTERS* exceptionPointers);

    // Generate unique crash ID
    static std::string generateCrashId();

    // Generate unique report ID
    static std::string generateReportId();

    // Get memory information
    static size_t getAvailableMemory();

    // Get disk space information
    static size_t getAvailableDiskSpace();

    // Get CPU core count
    static int getCpuCoreCount();

    // Get OS version string
    static std::string getOsVersion();

    // Get CPU information
    static std::string getCpuInfo();

    // Get GPU information
    static std::string getGpuInfo();

    // Get memory usage information
    static nlohmann::json getMemoryInfo();

    // Get build architecture
    static std::string getArchitecture();

    // Format stack trace as string
    static std::string formatStackTrace(const std::stacktrace& trace);

    // Determine crash type from signal
    static std::string getCrashType(int signal);

    // Send telemetry asynchronously
    static void sendTelemetryAsync(const nlohmann::json& payload);

    // Get latest log file content
    static std::string getLatestLogContent();

    // Get enabled modules as text (like in crash log file)
    static std::string getEnabledModulesText();

    // Session start time
    static std::chrono::steady_clock::time_point s_sessionStart;
    static bool s_initialized;
};