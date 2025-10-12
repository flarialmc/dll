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
    const std::string& signalName,
    EXCEPTION_POINTERS* exceptionPointers
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
            {"crashInfo", getCrashInfo(trace, signal, signalName, exceptionPointers)},
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
    const std::string& signalName,
    EXCEPTION_POINTERS* exceptionPointers
) {
    std::string message = signalName.empty() ? "Unknown crash" : signalName;

    // Add exception details to the message string
    if (exceptionPointers && exceptionPointers->ExceptionRecord) {
        auto* record = exceptionPointers->ExceptionRecord;

        std::stringstream ss;
        ss << message << "\n";

        // Get exception name
        std::string exceptionName;
        switch (record->ExceptionCode) {
            case EXCEPTION_ACCESS_VIOLATION: exceptionName = "EXCEPTION_ACCESS_VIOLATION"; break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: exceptionName = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
            case EXCEPTION_BREAKPOINT: exceptionName = "EXCEPTION_BREAKPOINT"; break;
            case EXCEPTION_DATATYPE_MISALIGNMENT: exceptionName = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
            case EXCEPTION_FLT_DENORMAL_OPERAND: exceptionName = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: exceptionName = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
            case EXCEPTION_FLT_INEXACT_RESULT: exceptionName = "EXCEPTION_FLT_INEXACT_RESULT"; break;
            case EXCEPTION_FLT_INVALID_OPERATION: exceptionName = "EXCEPTION_FLT_INVALID_OPERATION"; break;
            case EXCEPTION_FLT_OVERFLOW: exceptionName = "EXCEPTION_FLT_OVERFLOW"; break;
            case EXCEPTION_FLT_STACK_CHECK: exceptionName = "EXCEPTION_FLT_STACK_CHECK"; break;
            case EXCEPTION_FLT_UNDERFLOW: exceptionName = "EXCEPTION_FLT_UNDERFLOW"; break;
            case EXCEPTION_ILLEGAL_INSTRUCTION: exceptionName = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
            case EXCEPTION_IN_PAGE_ERROR: exceptionName = "EXCEPTION_IN_PAGE_ERROR"; break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO: exceptionName = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
            case EXCEPTION_INT_OVERFLOW: exceptionName = "EXCEPTION_INT_OVERFLOW"; break;
            case EXCEPTION_INVALID_DISPOSITION: exceptionName = "EXCEPTION_INVALID_DISPOSITION"; break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: exceptionName = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
            case EXCEPTION_PRIV_INSTRUCTION: exceptionName = "EXCEPTION_PRIV_INSTRUCTION"; break;
            case EXCEPTION_SINGLE_STEP: exceptionName = "EXCEPTION_SINGLE_STEP"; break;
            case EXCEPTION_STACK_OVERFLOW: exceptionName = "EXCEPTION_STACK_OVERFLOW"; break;
            default: exceptionName = "UNKNOWN_EXCEPTION"; break;
        }

        ss << "Exception Code: 0x" << std::hex << std::setw(16) << std::setfill('0') << record->ExceptionCode
           << " (" << exceptionName << ")\n";
        ss << "Exception Address: 0x" << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)record->ExceptionAddress << "\n";
        ss << "Exception Flags: 0x" << std::hex << std::setw(16) << std::setfill('0') << record->ExceptionFlags;

        // Additional info for access violations
        if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
            std::string accessType;
            if (record->ExceptionInformation[0] == 0) accessType = "Read";
            else if (record->ExceptionInformation[0] == 1) accessType = "Write";
            else accessType = "Execute";

            ss << "\n\nAccess Violation Details:\n";
            ss << "  Type: " << accessType << "\n";
            ss << "  Address: 0x" << std::hex << std::setw(16) << std::setfill('0') << record->ExceptionInformation[1];
        }

        message = ss.str();
    }

    nlohmann::json crashInfo = {
        {"type", getCrashType(signal)},
        {"message", message},
        {"stackTrace", formatStackTrace(trace)}
    };

    if (signal != 0) {
        crashInfo["fatalSignal"] = signalName;
        crashInfo["exitCode"] = signal;
    }

    return crashInfo;
}

nlohmann::json CrashTelemetry::getExceptionDetails(EXCEPTION_POINTERS* exceptionPointers) {
    if (!exceptionPointers || !exceptionPointers->ExceptionRecord) {
        return nullptr;
    }

    auto* record = exceptionPointers->ExceptionRecord;

    nlohmann::json details = {
        {"code", record->ExceptionCode},
        {"address", (uint64_t)record->ExceptionAddress},
        {"flags", record->ExceptionFlags}
    };

    // Get exception name
    std::string exceptionName;
    switch (record->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION: exceptionName = "EXCEPTION_ACCESS_VIOLATION"; break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: exceptionName = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
        case EXCEPTION_BREAKPOINT: exceptionName = "EXCEPTION_BREAKPOINT"; break;
        case EXCEPTION_DATATYPE_MISALIGNMENT: exceptionName = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
        case EXCEPTION_FLT_DENORMAL_OPERAND: exceptionName = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO: exceptionName = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
        case EXCEPTION_FLT_INEXACT_RESULT: exceptionName = "EXCEPTION_FLT_INEXACT_RESULT"; break;
        case EXCEPTION_FLT_INVALID_OPERATION: exceptionName = "EXCEPTION_FLT_INVALID_OPERATION"; break;
        case EXCEPTION_FLT_OVERFLOW: exceptionName = "EXCEPTION_FLT_OVERFLOW"; break;
        case EXCEPTION_FLT_STACK_CHECK: exceptionName = "EXCEPTION_FLT_STACK_CHECK"; break;
        case EXCEPTION_FLT_UNDERFLOW: exceptionName = "EXCEPTION_FLT_UNDERFLOW"; break;
        case EXCEPTION_ILLEGAL_INSTRUCTION: exceptionName = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
        case EXCEPTION_IN_PAGE_ERROR: exceptionName = "EXCEPTION_IN_PAGE_ERROR"; break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO: exceptionName = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
        case EXCEPTION_INT_OVERFLOW: exceptionName = "EXCEPTION_INT_OVERFLOW"; break;
        case EXCEPTION_INVALID_DISPOSITION: exceptionName = "EXCEPTION_INVALID_DISPOSITION"; break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: exceptionName = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
        case EXCEPTION_PRIV_INSTRUCTION: exceptionName = "EXCEPTION_PRIV_INSTRUCTION"; break;
        case EXCEPTION_SINGLE_STEP: exceptionName = "EXCEPTION_SINGLE_STEP"; break;
        case EXCEPTION_STACK_OVERFLOW: exceptionName = "EXCEPTION_STACK_OVERFLOW"; break;
        default: exceptionName = "UNKNOWN_EXCEPTION"; break;
    }
    details["name"] = exceptionName;

    // Additional info for access violations
    if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
        std::string accessType;
        if (record->ExceptionInformation[0] == 0) accessType = "Read";
        else if (record->ExceptionInformation[0] == 1) accessType = "Write";
        else accessType = "Execute";

        details["accessViolation"] = {
            {"type", accessType},
            {"address", (uint64_t)record->ExceptionInformation[1]}
        };
    }

    return details;
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