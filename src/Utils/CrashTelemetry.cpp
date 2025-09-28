#include "CrashTelemetry.hpp"
#include "APIUtils.hpp"
#include "Telemetry.hpp"
#include "Logger/Logger.hpp"
#include "UserActionLogger.hpp"
#include "Utils.hpp"
#include <json/json.hpp>
#include <windows.h>
#include <psapi.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <filesystem>
#include <csignal>
#include <fstream>
#include "Client/Module/Manager.hpp"
#include "Client/Client.hpp"
#include "Client/Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "version.lib")

std::chrono::steady_clock::time_point CrashTelemetry::s_sessionStart;
bool CrashTelemetry::s_initialized = false;

void CrashTelemetry::initialize() {
    if (s_initialized) return;

    s_sessionStart = std::chrono::steady_clock::now();
    s_initialized = true;

    Logger::info("CrashTelemetry initialized");
}

void CrashTelemetry::sendCrashReport(
    const std::stacktrace& trace,
    int signal,
    const std::string& signalName
) {
    try {
        if (!s_initialized) {
            initialize();
        }

        // Generate crash and report IDs
        std::string crashId = generateCrashId();
        std::string reportId = generateReportId();

        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();

        // Build crash report JSON according to specification
        nlohmann::json crashReport = {
            {"crashId", crashId},
            {"timestamp", timestamp},
            {"clientInfo", getClientInfo()},
            {"crashInfo", getCrashInfo(trace, signal, signalName)},
            {"systemInfo", getSystemInfo()},
            {"hardwareInfo", getHardwareInfo()},
            {"sessionInfo", getSessionInfo()},
            {"additionalData", {
                {"logFiles", nlohmann::json::array()}, // Could be populated later
                {"moduleStates", getModuleStates()},
                {"userActions", UserActionLogger::getRecentActions(100)}, // Match exportToFile behavior
                {"logs", getLatestLogContent()},
                {"commitHash", COMMIT_HASH}, // Add commit hash like in crash log file
                {"enabledModulesText", getEnabledModulesText()} // Add enabled modules list like in crash log file
            }}
        };

        // Send telemetry asynchronously
        sendTelemetryAsync(crashReport);

        Logger::info("Crash telemetry sent - CrashID: {}, ReportID: {}", crashId, reportId);
    }
    catch (const std::exception& e) {
        Logger::warn("Failed to send crash telemetry: {}", e.what());
    }
}

nlohmann::json CrashTelemetry::getClientInfo() {
    nlohmann::json clientInfo = {
        {"version", Telemetry::getClientVersion()},
        {"platform", "windows"},
        {"architecture", getArchitecture()},
        {"buildNumber", "dev"}, // Could be made configurable
        {"commitHash", COMMIT_HASH},
        {"DirectXVersion", SwapchainHook::isDX12 ? "DX12" : "DX11"}
    };

    // Add client config settings
    try {
        clientInfo["config"] = {
            {"killdx", Client::settings.getSettingByName<bool>("killdx")->value},
            {"vsync", Client::settings.getSettingByName<bool>("vsync")->value},
            {"recreateAtStart", Client::settings.getSettingByName<bool>("recreateAtStart")->value}
        };
    } catch (const std::exception& e) {
        Logger::warn("Failed to get client config settings: {}", e.what());
        clientInfo["config"] = {
            {"killdx", "error"},
            {"vsync", "error"},
            {"recreateAtStart", "error"}
        };
    }

    return clientInfo;
}

nlohmann::json CrashTelemetry::getCrashInfo(
    const std::stacktrace& trace,
    int signal,
    const std::string& signalName
) {
    nlohmann::json crashInfo = {
        {"type", getCrashType(signal)},
        {"message", signalName.empty() ? "Unknown crash" : signalName},
        {"stackTrace", formatStackTrace(trace)}
    };

    if (signal != 0) {
        crashInfo["fatalSignal"] = signalName;
        crashInfo["exitCode"] = signal;
    }

    return crashInfo;
}

nlohmann::json CrashTelemetry::getSystemInfo() {
    return {
        {"osVersion", getOsVersion()},
        {"memory", getAvailableMemory()},
        {"cpuCores", getCpuCoreCount()},
        {"diskSpace", getAvailableDiskSpace()}
    };
}

nlohmann::json CrashTelemetry::getSessionInfo() {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
        now - s_sessionStart
    ).count();

    return {
        {"sessionId", generateReportId()}, // Generate a session ID
        {"uptime", uptime},
        {"username", Telemetry::generateUserHash()}, // Use existing hash system
        {"serverConnected", ""} // Could be populated if server info is available
    };
}

nlohmann::json CrashTelemetry::getModuleStates() {
    nlohmann::json moduleStates = nlohmann::json::object();

    try {
        for (const auto& pair : ModuleManager::moduleMap) {
            if (pair.second && pair.second->isEnabled()) {
                moduleStates[pair.second->name] = {
                    {"enabled", true},
                    {"keybind", pair.second->settings.getSettingByName<std::string>("keybind") ?
                               pair.second->settings.getSettingByName<std::string>("keybind")->value : ""}
                };
            }
        }
    } catch (const std::exception& e) {
        Logger::warn("Failed to get module states: {}", e.what());
    }

    return moduleStates;
}

nlohmann::json CrashTelemetry::getHardwareInfo() {
    return {
        {"cpu", getCpuInfo()},
        {"gpu", getGpuInfo()},
        {"memory", getMemoryInfo()}
    };
}

std::string CrashTelemetry::generateCrashId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;

    // Generate UUID-like format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            ss << "-";
        }
        ss << dis(gen);
    }

    return ss.str();
}

std::string CrashTelemetry::generateReportId() {
    return generateCrashId(); // Same format for simplicity
}

size_t CrashTelemetry::getAvailableMemory() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo)) {
        return static_cast<size_t>(memInfo.ullAvailPhys);
    }

    return 0;
}

size_t CrashTelemetry::getAvailableDiskSpace() {
    ULARGE_INTEGER freeBytesAvailable;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, nullptr, nullptr)) {
        return static_cast<size_t>(freeBytesAvailable.QuadPart);
    }

    return 0;
}

int CrashTelemetry::getCpuCoreCount() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<int>(sysInfo.dwNumberOfProcessors);
}

std::string CrashTelemetry::getOsVersion() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    // Note: GetVersionEx is deprecated, but still works for basic info
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        std::stringstream ss;
        ss << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
           << " Build " << osvi.dwBuildNumber;
        return ss.str();
    }

    return "Windows (Unknown Version)";
}

std::string CrashTelemetry::getArchitecture() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "x64";
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "x86";
        case PROCESSOR_ARCHITECTURE_ARM64:
            return "arm64";
        case PROCESSOR_ARCHITECTURE_ARM:
            return "arm";
        default:
            return "unknown";
    }
}

std::string CrashTelemetry::formatStackTrace(const std::stacktrace& trace) {
    std::stringstream ss;

    for (const auto& entry : trace) {
        ss << entry << "\n";
    }

    return ss.str();
}

std::string CrashTelemetry::getCrashType(int signal) {
    if (signal == SIGSEGV) {
        return "segfault";
    } else if (signal == SIGABRT) {
        return "assertion";
    } else if (signal == SIGFPE || signal == SIGILL) {
        return "exception";
    } else if (signal == SIGTERM || signal == SIGINT) {
        return "timeout";
    } else {
        return "exception";
    }
}

std::string CrashTelemetry::getCpuInfo() {
    try {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                          "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char cpuName[256];
            DWORD dataSize = sizeof(cpuName);
            if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr,
                                (LPBYTE)cpuName, &dataSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::string(cpuName);
            }
            RegCloseKey(hKey);
        }
    } catch (...) {}

    return "Unknown CPU";
}

std::string CrashTelemetry::getGpuInfo() {
    try {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                          "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char gpuName[256];
            DWORD dataSize = sizeof(gpuName);
            if (RegQueryValueExA(hKey, "DriverDesc", nullptr, nullptr,
                                (LPBYTE)gpuName, &dataSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::string(gpuName);
            }
            RegCloseKey(hKey);
        }
    } catch (...) {}

    return "Unknown GPU";
}

nlohmann::json CrashTelemetry::getMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo)) {
        return {
            {"totalPhysical", static_cast<size_t>(memInfo.ullTotalPhys)},
            {"availablePhysical", static_cast<size_t>(memInfo.ullAvailPhys)},
            {"usedPhysical", static_cast<size_t>(memInfo.ullTotalPhys - memInfo.ullAvailPhys)},
            {"memoryLoad", static_cast<int>(memInfo.dwMemoryLoad)}
        };
    }

    return {
        {"totalPhysical", 0},
        {"availablePhysical", 0},
        {"usedPhysical", 0},
        {"memoryLoad", 0}
    };
}

std::string CrashTelemetry::getLatestLogContent() {
    try {
        std::string logPath = Utils::getClientPath() + "\\logs\\latest.log";
        std::ifstream logFile(logPath);

        if (!logFile.is_open()) {
            return "Could not open latest.log file";
        }

        std::stringstream buffer;
        buffer << logFile.rdbuf();
        logFile.close();

        std::string content = buffer.str();

        // Limit log content to prevent huge payloads (last 50KB)
        const size_t maxSize = 50 * 1024; // 50KB
        if (content.size() > maxSize) {
            content = "... [truncated] ...\n" + content.substr(content.size() - maxSize);
        }

        return content;
    } catch (const std::exception& e) {
        return "Error reading latest.log: " + std::string(e.what());
    }
}

std::string CrashTelemetry::getEnabledModulesText() {
    try {
        std::stringstream ss;
        for (const auto& pair : ModuleManager::moduleMap) {
            if (pair.second && pair.second->isEnabled()) {
                ss << pair.second->name << "\n";
            }
        }
        return ss.str();
    } catch (const std::exception& e) {
        return "Error getting enabled modules: " + std::string(e.what());
    }
}

void CrashTelemetry::sendTelemetryAsync(const nlohmann::json& payload) {
    std::thread([payload]() {
        try {
            std::string jsonData = payload.dump();
            auto result = APIUtils::POST_Simple("https://api.flarial.xyz/api/v1/crash-logs", jsonData);

            if (result.first >= 200 && result.first < 300) {
                Logger::debug("Crash telemetry sent successfully: HTTP {}", result.first);
            } else {
                Logger::warn("Crash telemetry failed: HTTP {}", result.first);
            }
        } catch (const std::exception& e) {
            Logger::warn("Failed to send crash telemetry: {}", e.what());
        }
    }).detach();
}