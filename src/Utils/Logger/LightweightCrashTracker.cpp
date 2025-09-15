#include "LightweightCrashTracker.hpp"
#include "crashlogs.hpp"
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <unordered_map>
#include <Client/Module/Manager.hpp>
#include <Utils/Utils.hpp>

namespace CrashTracker {

    static thread_local std::vector<ExecutionContext> contextStack;
    static std::unordered_map<std::string, std::string> globalContext;
    static std::mutex globalMutex;
    static LPTOP_LEVEL_EXCEPTION_FILTER previousFilter = nullptr;

    std::string getTimestamp() {
        auto now = std::time(nullptr);
        auto tm = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string getExceptionName(DWORD code) {
        switch (code) {
            case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
            case EXCEPTION_BREAKPOINT: return "BREAKPOINT";
            case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
            case EXCEPTION_FLT_DENORMAL_OPERAND: return "FLOAT_DENORMAL_OPERAND";
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLOAT_DIVIDE_BY_ZERO";
            case EXCEPTION_FLT_INEXACT_RESULT: return "FLOAT_INEXACT_RESULT";
            case EXCEPTION_FLT_INVALID_OPERATION: return "FLOAT_INVALID_OPERATION";
            case EXCEPTION_FLT_OVERFLOW: return "FLOAT_OVERFLOW";
            case EXCEPTION_FLT_STACK_CHECK: return "FLOAT_STACK_CHECK";
            case EXCEPTION_FLT_UNDERFLOW: return "FLOAT_UNDERFLOW";
            case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
            case EXCEPTION_IN_PAGE_ERROR: return "IN_PAGE_ERROR";
            case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INTEGER_DIVIDE_BY_ZERO";
            case EXCEPTION_INT_OVERFLOW: return "INTEGER_OVERFLOW";
            case EXCEPTION_INVALID_DISPOSITION: return "INVALID_DISPOSITION";
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
            case EXCEPTION_PRIV_INSTRUCTION: return "PRIVILEGED_INSTRUCTION";
            case EXCEPTION_SINGLE_STEP: return "SINGLE_STEP";
            case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
            default: {
                std::stringstream ss;
                ss << "UNKNOWN_0x" << std::hex << code;
                return ss.str();
            }
        }
    }

    std::string generateCrashReport() {
        std::stringstream report;

        report << "========================================\n";
        report << "    FLARIAL LIGHTWEIGHT CRASH REPORT\n";
        report << "========================================\n\n";

        report << "Timestamp: " << getTimestamp() << "\n\n";

        // Global context
        if (!globalContext.empty()) {
            report << "Global Context:\n";
            report << "----------------------------------------\n";
            std::lock_guard<std::mutex> lock(globalMutex);
            for (const auto& [key, value] : globalContext) {
                report << "  " << key << ": " << value << "\n";
            }
            report << "\n";
        }

        // Execution context stack
        report << "Execution Context (Most Recent First):\n";
        report << "----------------------------------------\n";
        if (contextStack.empty()) {
            report << "  (No context information available)\n";
        } else {
            int depth = 0;
            // Show in reverse order (most recent first)
            for (auto it = contextStack.rbegin(); it != contextStack.rend(); ++it) {
                report << "  [" << depth++ << "] ";

                // Extract just filename from module path
                std::filesystem::path modulePath(it->moduleName);
                std::string moduleFile = modulePath.filename().string();

                report << moduleFile;
                if (!it->functionName.empty()) {
                    report << " -> " << it->functionName;
                }
                if (it->lineNumber > 0) {
                    report << " (line " << it->lineNumber << ")";
                }
                if (!it->description.empty()) {
                    report << "\n      Context: " << it->description;
                }
                report << "\n";
            }
        }
        report << "\n";

        // Enabled modules
        report << "Active Modules:\n";
        report << "----------------------------------------\n";
        try {
            bool hasModules = false;
            for (const auto& [name, module] : ModuleManager::moduleMap) {
                if (module && module->isEnabled()) {
                    report << "  ✓ " << name << "\n";
                    hasModules = true;
                }
            }
            if (!hasModules) {
                report << "  (No modules enabled)\n";
            }
        } catch (...) {
            report << "  (Failed to retrieve module list)\n";
        }

        report << "\n";
        report << "========================================\n";
        report << "For detailed analysis, please provide:\n";
        report << "  1. This crash log\n";
        report << "  2. The exact steps to reproduce\n";
        report << "  3. Your game version\n";
        report << "========================================\n";

        return report.str();
    }

    LONG WINAPI exceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
        try {
            std::stringstream crashInfo;

            crashInfo << "\n!!!! CRASH DETECTED !!!!\n";
            crashInfo << "Exception: " << getExceptionName(exceptionInfo->ExceptionRecord->ExceptionCode) << "\n";
            crashInfo << "Address: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress << std::dec << "\n";

            if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
                ULONG_PTR* info = exceptionInfo->ExceptionRecord->ExceptionInformation;
                crashInfo << "Type: ";
                crashInfo << (info[0] == 0 ? "Read from" : info[0] == 1 ? "Write to" : "Execute at");
                crashInfo << " address 0x" << std::hex << info[1] << std::dec << "\n";
            }

            // Set global context with crash info
            setGlobalContext("Crash Type", getExceptionName(exceptionInfo->ExceptionRecord->ExceptionCode));

            std::stringstream addrStr;
            addrStr << "0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress;
            setGlobalContext("Crash Address", addrStr.str());

            // Generate and save the crash report
            std::string report = crashInfo.str() + "\n" + generateCrashReport();

            // Write to file
            std::string logsPath = Utils::getLogsPath();
            std::string timestamp = getTimestamp();
            std::replace(timestamp.begin(), timestamp.end(), ':', '-');
            std::replace(timestamp.begin(), timestamp.end(), ' ', '_');

            std::string filename = logsPath + "\\crash_lightweight_" + timestamp + ".txt";
            std::ofstream file(filename);
            if (file.is_open()) {
                file << report;
                file.close();
            }

            // Also output to console/debugger
            OutputDebugStringA(report.c_str());

        } catch (...) {
            // Silent fail - we're already crashing
        }

        // Call previous handler if it exists
        if (previousFilter) {
            return previousFilter(exceptionInfo);
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void initialize() {
        previousFilter = SetUnhandledExceptionFilter(exceptionHandler);

        // Set some initial global context
        setGlobalContext("Client Version", FLARIAL_VERSION);
        setGlobalContext("Build Type", FLARIAL_BUILD_TYPE);
        setGlobalContext("Build Date", FLARIAL_BUILD_DATE);
    }

    void shutdown() {
        if (previousFilter) {
            SetUnhandledExceptionFilter(previousFilter);
            previousFilter = nullptr;
        }
        contextStack.clear();
        globalContext.clear();
    }

    void pushContext(const std::string& module, const std::string& function, int line, const std::string& desc) {
        contextStack.push_back({module, function, desc, line});
    }

    void popContext() {
        if (!contextStack.empty()) {
            contextStack.pop_back();
        }
    }

    void setGlobalContext(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(globalMutex);
        globalContext[key] = value;
    }

    // RAII Guard implementation
    ContextGuard::ContextGuard(const std::string& module, const std::string& function, int line, const std::string& desc) {
        pushContext(module, function, line, desc);
    }

    ContextGuard::~ContextGuard() {
        popContext();
    }
}