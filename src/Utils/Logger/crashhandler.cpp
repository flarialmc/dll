#include "crashhandler.hpp"
#include <Windows.h>
#include <DbgHelp.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <vector>
#include <stacktrace>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <Client/Module/Manager.hpp>

#pragma comment(lib, "dbghelp.lib")

namespace CrashHandler {
    static std::string g_crashLogPath;
    static std::function<void(const std::string&)> g_customCallback;
    static LPTOP_LEVEL_EXCEPTION_FILTER g_previousFilter = nullptr;
    static std::mutex g_mutex;
    static std::condition_variable g_cv;
    static std::thread g_crashThread;
    static std::atomic<bool> g_crashOccurred(false);
    static EXCEPTION_POINTERS* g_exceptionInfo = nullptr;

    std::string getExceptionName(DWORD code) {
        switch (code) {
            case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
            case EXCEPTION_BREAKPOINT: return "BREAKPOINT";
            case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
            case EXCEPTION_FLT_DENORMAL_OPERAND: return "FLT_DENORMAL_OPERAND";
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLT_DIVIDE_BY_ZERO";
            case EXCEPTION_FLT_INEXACT_RESULT: return "FLT_INEXACT_RESULT";
            case EXCEPTION_FLT_INVALID_OPERATION: return "FLT_INVALID_OPERATION";
            case EXCEPTION_FLT_OVERFLOW: return "FLT_OVERFLOW";
            case EXCEPTION_FLT_STACK_CHECK: return "FLT_STACK_CHECK";
            case EXCEPTION_FLT_UNDERFLOW: return "FLT_UNDERFLOW";
            case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
            case EXCEPTION_IN_PAGE_ERROR: return "IN_PAGE_ERROR";
            case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INT_DIVIDE_BY_ZERO";
            case EXCEPTION_INT_OVERFLOW: return "INT_OVERFLOW";
            case EXCEPTION_INVALID_DISPOSITION: return "INVALID_DISPOSITION";
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
            case EXCEPTION_PRIV_INSTRUCTION: return "PRIV_INSTRUCTION";
            case EXCEPTION_SINGLE_STEP: return "SINGLE_STEP";
            case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
            default: {
                std::stringstream ss;
                ss << "UNKNOWN_EXCEPTION_0x" << std::hex << code;
                return ss.str();
            }
        }
    }

    std::string getTimestamp() {
        auto now = std::time(nullptr);
        auto tm = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d_%H-%M-%S");
        return ss.str();
    }

    struct SymbolInfo {
        std::string moduleName;
        std::string functionName;
        std::string fileName;
        DWORD lineNumber;
        DWORD64 displacement;
        DWORD64 address;
    };

    SymbolInfo resolveSymbol(HANDLE process, DWORD64 address) {
        SymbolInfo info;
        info.address = address;
        info.lineNumber = 0;
        info.displacement = 0;

        // Get module info
        IMAGEHLP_MODULE64 moduleInfo = {};
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);
        if (SymGetModuleInfo64(process, address, &moduleInfo)) {
            info.moduleName = moduleInfo.ModuleName;
        }

        // Get symbol info
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        if (SymFromAddr(process, address, &info.displacement, symbol)) {
            info.functionName = symbol->Name;
        } else {
            std::stringstream ss;
            ss << "0x" << std::hex << address;
            info.functionName = ss.str();
        }

        // Get line info
        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(line);
        DWORD displacement = 0;
        if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
            info.fileName = line.FileName;
            info.lineNumber = line.LineNumber;
        }

        return info;
    }

    std::string generateStackTrace(EXCEPTION_POINTERS* exceptionInfo) {
        std::stringstream ss;
        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        // Initialize symbol handler
        SymInitialize(process, nullptr, TRUE);
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

        // Get context from exception
        CONTEXT context = *exceptionInfo->ContextRecord;

        // Setup stack frame
        STACKFRAME64 stackFrame = {};
        DWORD machineType;

#ifdef _M_AMD64
        machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rbp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IX86
        machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context.Eip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Ebp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Esp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#else
#error "Unsupported platform"
#endif

        ss << "Stack Trace:\n";
        ss << "============================================================\n";

        int frameNumber = 0;
        while (StackWalk64(machineType, process, thread, &stackFrame, &context,
                          nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {

            if (stackFrame.AddrPC.Offset == 0) break;

            SymbolInfo symbolInfo = resolveSymbol(process, stackFrame.AddrPC.Offset);

            ss << "[" << frameNumber++ << "] ";

            if (!symbolInfo.moduleName.empty()) {
                ss << symbolInfo.moduleName << "!";
            }

            ss << symbolInfo.functionName;

            if (symbolInfo.displacement > 0) {
                ss << "+0x" << std::hex << symbolInfo.displacement << std::dec;
            }

            if (!symbolInfo.fileName.empty() && symbolInfo.lineNumber > 0) {
                ss << "\n    at " << symbolInfo.fileName << ":" << symbolInfo.lineNumber;
            }

            ss << "\n    Address: 0x" << std::hex << symbolInfo.address << std::dec << "\n\n";
        }

        SymCleanup(process);
        return ss.str();
    }

    std::string generateFullReport(EXCEPTION_POINTERS* exceptionInfo) {
        std::stringstream report;

        report << "========================================\n";
        report << "          FLARIAL CRASH REPORT\n";
        report << "========================================\n\n";

        report << "Timestamp: " << getTimestamp() << "\n";
        report << "Exception Code: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode << std::dec;
        report << " (" << getExceptionName(exceptionInfo->ExceptionRecord->ExceptionCode) << ")\n";
        report << "Exception Address: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress << std::dec << "\n";

        if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
            ULONG_PTR* info = exceptionInfo->ExceptionRecord->ExceptionInformation;
            report << "Access Violation: ";
            report << (info[0] == 0 ? "Read" : info[0] == 1 ? "Write" : "Execute");
            report << " at address 0x" << std::hex << info[1] << std::dec << "\n";
        }

        report << "\n";

        // Add stack trace using C++23 stacktrace as fallback
        try {
            report << "C++23 Stack Trace:\n";
            report << "============================================================\n";
            auto trace = std::stacktrace::current();
            report << trace << "\n\n";
        } catch (...) {
            report << "Failed to generate C++23 stack trace\n\n";
        }

        // Add detailed stack trace with symbols
        report << generateStackTrace(exceptionInfo);

        // Add enabled modules
        report << "\nEnabled Modules:\n";
        report << "============================================================\n";
        try {
            for (const auto& [name, module] : ModuleManager::moduleMap) {
                if (module->isEnabled()) {
                    report << "  - " << name << "\n";
                }
            }
        } catch (...) {
            report << "  (Failed to retrieve module list)\n";
        }

        // System information
        report << "\nSystem Information:\n";
        report << "============================================================\n";

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        report << "Processor Count: " << sysInfo.dwNumberOfProcessors << "\n";

        MEMORYSTATUSEX memInfo = {};
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo)) {
            report << "Memory Load: " << memInfo.dwMemoryLoad << "%\n";
            report << "Total Physical Memory: " << (memInfo.ullTotalPhys / (1024 * 1024)) << " MB\n";
            report << "Available Physical Memory: " << (memInfo.ullAvailPhys / (1024 * 1024)) << " MB\n";
        }

        return report.str();
    }

    void writeCrashLog(const std::string& report) {
        std::string filename = "crash_" + getTimestamp() + ".txt";
        std::filesystem::path crashPath = g_crashLogPath.empty() ?
            std::filesystem::current_path() : std::filesystem::path(g_crashLogPath);

        if (!std::filesystem::exists(crashPath)) {
            std::filesystem::create_directories(crashPath);
        }

        std::filesystem::path fullPath = crashPath / filename;
        std::ofstream file(fullPath);
        if (file.is_open()) {
            file << report;
            file.close();
        }

        if (g_customCallback) {
            g_customCallback(fullPath.string());
        }
    }

    void crashHandlerThread() {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_cv.wait(lock, []{ return g_crashOccurred.load(); });

        if (g_exceptionInfo) {
            std::string report = generateFullReport(g_exceptionInfo);
            writeCrashLog(report);
        }

        g_crashOccurred = false;
        g_cv.notify_all();
    }

    LONG WINAPI exceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
        if (g_crashOccurred.exchange(true)) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        g_exceptionInfo = exceptionInfo;
        g_cv.notify_all();

        std::unique_lock<std::mutex> lock(g_mutex);
        g_cv.wait(lock, []{ return !g_crashOccurred.load(); });

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void initialize(const std::string& crashLogPath) {
        g_crashLogPath = crashLogPath;
        g_previousFilter = SetUnhandledExceptionFilter(exceptionHandler);
        g_crashThread = std::thread(crashHandlerThread);
    }

    void setCustomCallback(std::function<void(const std::string&)> callback) {
        g_customCallback = callback;
    }

    void shutdown() {
        if (g_previousFilter) {
            SetUnhandledExceptionFilter(g_previousFilter);
            g_previousFilter = nullptr;
        }

        g_crashOccurred = true;
        g_cv.notify_all();

        if (g_crashThread.joinable()) {
            g_crashThread.join();
        }
    }

    std::string generateReport() {
        std::stringstream report;
        report << "========================================\n";
        report << "      FLARIAL DIAGNOSTIC REPORT\n";
        report << "========================================\n\n";
        report << "Timestamp: " << getTimestamp() << "\n\n";

        try {
            report << "Current Stack Trace:\n";
            report << "============================================================\n";
            auto trace = std::stacktrace::current();
            report << trace << "\n";
        } catch (...) {
            report << "Failed to generate stack trace\n";
        }

        return report.str();
    }
}