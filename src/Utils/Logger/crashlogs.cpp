#include "crashlogs.hpp"

#include <Client/Module/Manager.hpp>
#include <kiero/kiero.h>
#include <Utils/CrashTelemetry.hpp>
#include <Utils/UserActionLogger.hpp>

//needed to get a stack trace
#include <stacktrace>

//needed for threading (threading needed to be able to output a call stack during a stack overflow)
#include <thread>
#include <mutex>
#include <atomic>

//needed for being able to output a timestampped crash log
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

//needed for being able to get into the crash handler on a crash
#include <csignal>
#include <exception>
#include <cstdlib>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

//a decent amount of this was copied/modified from backward.cpp (https://github.com/bombela/backward-cpp)
//mostly the stuff related to actually getting crash handlers on crashes
//and the thread which is SOLELY there to be able to write a log on a stack overflow,
//since otherwise there is not enough stack space to output the stack trace
//main difference here is utilizing C++23 <stacktrace> header for generating stack traces
//and using <atomic> and a few other more recent C++ features if we're gonna be using C++23 anyway

namespace glaiel::crashlogs {
    //information for where to save stack traces
    static std::stacktrace trace;
    static std::string header_message;
    static int crash_signal = 0; // 0 is not a valid signal id
    static std::filesystem::path output_folder;
    static std::string filename = "crash_{timestamp}.txt";
    static void (*on_output_crashlog)(std::string crashlog_filename) = NULL;

    //exception information
    static EXCEPTION_POINTERS* exception_pointers = nullptr;

    //thread stuff
    static std::mutex mut;
    static std::condition_variable cv;
    static std::thread output_thread;
    enum class program_status {
        running = 0,
        crashed = 1,
        ending = 2,
        normal_exit = 3,
        end_session = 4
    };
    static std::atomic<program_status> status = program_status::running;

    //public interface (see header for documentation)
    void set_crashlog_folder(std::string folderpath) {
        output_folder = folderpath;
    }

    static std::string current_timestamp();
    static std::filesystem::path get_log_filepath();
    static const char* try_get_signal_name(int signal);
    static std::string get_exception_name(DWORD code);
    static std::string format_hex(DWORD64 value);
    static std::string get_register_dump(CONTEXT* ctx);
    static std::string get_loaded_modules();
    static std::string get_memory_dump(void* address);
    static std::string get_exception_info();

    //output the crashlog file after a crash has occured
    static void output_crash_log() {
        std::filesystem::path path = get_log_filepath();
        std::ofstream log(path);

        // Basic information
        log << "===============================================\n";
        log << "           CRASH REPORT\n";
        log << "===============================================\n\n";
        log << "COMMIT_HASH: " << COMMIT_HASH << std::endl;
        log << "Timestamp: " << current_timestamp() << "\n";
        log << "Process ID: " << GetCurrentProcessId() << "\n";
        log << "Thread ID: " << GetCurrentThreadId() << "\n\n";

        if(!header_message.empty()) {
            log << header_message << std::endl << std::endl;
        }

        // Exception/Signal information
        log << "===============================================\n";
        log << "           EXCEPTION DETAILS\n";
        log << "===============================================\n\n";

        if(crash_signal != 0) {
            log << "Signal: " << crash_signal << " (" << try_get_signal_name(crash_signal) << ")\n";
        }

        if(exception_pointers != nullptr) {
            log << get_exception_info() << "\n";
        }
        log << "\n";

        // Stack trace with enhanced formatting
        log << "===============================================\n";
        log << "           STACK TRACE\n";
        log << "===============================================\n\n";

        size_t frame_num = 0;
        for (const auto& entry : trace) {
            log << "#" << std::setw(2) << std::setfill('0') << frame_num++ << " ";
            log << entry << "\n";
        }
        log << "\n";

        // Enabled modules
        log << "===============================================\n";
        log << "           ENABLED FLARIAL MODULES\n";
        log << "===============================================\n\n";

        for (const auto& pair : ModuleManager::moduleMap) {
            if(pair.second->isEnabled())
                log << "  - " << pair.second->name << "\n";
        }
        log << "\n";

        log.close();

        // Send crash telemetry
        try {
            std::string signalName = try_get_signal_name(crash_signal);
            CrashTelemetry::sendCrashReport(trace, crash_signal, signalName, exception_pointers);

#if defined(__DEBUG__)
            // Also export user actions to file for manual review (debug only)
            // UserActionLogger::exportToFile();
#endif
        } catch (const std::exception& e) {
            // Don't let telemetry errors affect crash handling
        }

        if(on_output_crashlog) on_output_crashlog(path.string());
    }

    //get the current timestamp as a string, for the crash log filename
    static std::string current_timestamp() {
        std::time_t rawtime;
        std::tm* timeinfo;
        char buffer[80];

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);

        std::strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", timeinfo);
        return buffer;
    }

    //utility function needed for crash log timestamps (replace {timestamp} in filename format with the timestamp string)
    static std::string replace_substr(std::string str, const std::string& search, const std::string& replace) {
        if(search.empty()) return str;
        size_t pos = 0;
        while((pos = str.find(search, pos)) != std::string::npos) {
            str.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return str;
    }

    //get the crash log filename
    std::filesystem::path get_log_filepath() {
        std::string timestampstr = current_timestamp();
        std::string timestampped_filename = replace_substr(filename, "{timestamp}", timestampstr);

        std::filesystem::path filepath = output_folder / timestampped_filename;

        //ensure the crash log folder exists. error code is here to supporess errors... since we're in an error handler already
        //at default settings this errors because an empty path is specified... lol. but we dont want to create a folder in that case anyway
        std::error_code err;
        std::filesystem::create_directories(output_folder, err);

        return filepath;
    }

    //using a thread here is a hack to get stack space in the case where the crash is a stack overflow
    //this hack was borrowed from backward.cpp
    static void crash_handler_thread() {
        //wait for the program to crash or exit normally
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [] { return status != program_status::running; });
        lk.unlock();

        if(status == program_status::end_session)
            return;

        //if it crashed, output the crash log
        if(status == program_status::crashed) {
            output_crash_log();
        }

        //alert the crashing thread we're done with the crash log so it can finish crashing
        status = program_status::ending;
        cv.notify_one();
    }

    static inline void crash_handler() {
        if(status == program_status::end_session)
            return;
        //if we crashed during a crash... ignore lol
        if(status != program_status::running) return;

        //save the stacktrace
        trace = std::stacktrace::current();

        //resume the monitoring thread
        status = program_status::crashed;
        cv.notify_one();

        //wait for the crash log to finish writing
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [] { return status != program_status::crashed; });
    }

    //Try to get the string representation of a signal identifier, return an empty string if none is found.
    //This only covers the signals from the C++ std lib and none of the POSIX or OS specific signal names!
    static const char* try_get_signal_name(int signal) {
        switch (signal) {
            case SIGTERM:
                return "SIGTERM";
            case SIGSEGV:
                return "SIGSEGV";
            case SIGINT:
                return "SIGINT";
            case SIGILL:
                return "SIGILL";
            case SIGABRT:
                return "SIGABRT";
            case SIGFPE:
                return "SIGFPE";
        }
        return "";
    }

    //Get human-readable exception name
    static std::string get_exception_name(DWORD code) {
        switch (code) {
            case EXCEPTION_ACCESS_VIOLATION: return "EXCEPTION_ACCESS_VIOLATION";
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
            case EXCEPTION_BREAKPOINT: return "EXCEPTION_BREAKPOINT";
            case EXCEPTION_DATATYPE_MISALIGNMENT: return "EXCEPTION_DATATYPE_MISALIGNMENT";
            case EXCEPTION_FLT_DENORMAL_OPERAND: return "EXCEPTION_FLT_DENORMAL_OPERAND";
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
            case EXCEPTION_FLT_INEXACT_RESULT: return "EXCEPTION_FLT_INEXACT_RESULT";
            case EXCEPTION_FLT_INVALID_OPERATION: return "EXCEPTION_FLT_INVALID_OPERATION";
            case EXCEPTION_FLT_OVERFLOW: return "EXCEPTION_FLT_OVERFLOW";
            case EXCEPTION_FLT_STACK_CHECK: return "EXCEPTION_FLT_STACK_CHECK";
            case EXCEPTION_FLT_UNDERFLOW: return "EXCEPTION_FLT_UNDERFLOW";
            case EXCEPTION_ILLEGAL_INSTRUCTION: return "EXCEPTION_ILLEGAL_INSTRUCTION";
            case EXCEPTION_IN_PAGE_ERROR: return "EXCEPTION_IN_PAGE_ERROR";
            case EXCEPTION_INT_DIVIDE_BY_ZERO: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
            case EXCEPTION_INT_OVERFLOW: return "EXCEPTION_INT_OVERFLOW";
            case EXCEPTION_INVALID_DISPOSITION: return "EXCEPTION_INVALID_DISPOSITION";
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
            case EXCEPTION_PRIV_INSTRUCTION: return "EXCEPTION_PRIV_INSTRUCTION";
            case EXCEPTION_SINGLE_STEP: return "EXCEPTION_SINGLE_STEP";
            case EXCEPTION_STACK_OVERFLOW: return "EXCEPTION_STACK_OVERFLOW";
            default: return "UNKNOWN_EXCEPTION";
        }
    }

    //Format hex value
    static std::string format_hex(DWORD64 value) {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << value;
        return ss.str();
    }

    //Get exception information
    static std::string get_exception_info() {
        if (!exception_pointers || !exception_pointers->ExceptionRecord) {
            return "No exception record available";
        }

        std::stringstream ss;
        auto* record = exception_pointers->ExceptionRecord;

        ss << "Exception Code: " << format_hex(record->ExceptionCode)
           << " (" << get_exception_name(record->ExceptionCode) << ")\n";
        ss << "Exception Address: " << format_hex((DWORD64)record->ExceptionAddress) << "\n";
        ss << "Exception Flags: " << format_hex(record->ExceptionFlags);

        if (record->ExceptionFlags & EXCEPTION_NONCONTINUABLE) {
            ss << " (NONCONTINUABLE)";
        }
        ss << "\n";

        // Additional info for access violations
        if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
            ss << "\nAccess Violation Details:\n";
            ss << "  Type: " << (record->ExceptionInformation[0] == 0 ? "Read" :
                                  record->ExceptionInformation[0] == 1 ? "Write" : "Execute") << "\n";
            ss << "  Address: " << format_hex(record->ExceptionInformation[1]) << "\n";
        }

        return ss.str();
    }

    //Get register dump
    static std::string get_register_dump(CONTEXT* ctx) {
        if (!ctx) return "No context available";

        std::stringstream ss;

#ifdef _WIN64
        ss << "RAX: " << format_hex(ctx->Rax) << "  RBX: " << format_hex(ctx->Rbx) << "\n";
        ss << "RCX: " << format_hex(ctx->Rcx) << "  RDX: " << format_hex(ctx->Rdx) << "\n";
        ss << "RSI: " << format_hex(ctx->Rsi) << "  RDI: " << format_hex(ctx->Rdi) << "\n";
        ss << "RBP: " << format_hex(ctx->Rbp) << "  RSP: " << format_hex(ctx->Rsp) << "\n";
        ss << "R8:  " << format_hex(ctx->R8)  << "  R9:  " << format_hex(ctx->R9) << "\n";
        ss << "R10: " << format_hex(ctx->R10) << "  R11: " << format_hex(ctx->R11) << "\n";
        ss << "R12: " << format_hex(ctx->R12) << "  R13: " << format_hex(ctx->R13) << "\n";
        ss << "R14: " << format_hex(ctx->R14) << "  R15: " << format_hex(ctx->R15) << "\n";
        ss << "RIP: " << format_hex(ctx->Rip) << "\n";
        ss << "EFLAGS: " << format_hex(ctx->EFlags) << "\n";
#else
        ss << "EAX: " << format_hex(ctx->Eax) << "  EBX: " << format_hex(ctx->Ebx) << "\n";
        ss << "ECX: " << format_hex(ctx->Ecx) << "  EDX: " << format_hex(ctx->Edx) << "\n";
        ss << "ESI: " << format_hex(ctx->Esi) << "  EDI: " << format_hex(ctx->Edi) << "\n";
        ss << "EBP: " << format_hex(ctx->Ebp) << "  ESP: " << format_hex(ctx->Esp) << "\n";
        ss << "EIP: " << format_hex(ctx->Eip) << "\n";
        ss << "EFLAGS: " << format_hex(ctx->EFlags) << "\n";
#endif

        return ss.str();
    }

    //Get loaded modules information
    static std::string get_loaded_modules() {
        std::stringstream ss;
        HANDLE process = GetCurrentProcess();
        HMODULE modules[1024];
        DWORD needed;

        if (EnumProcessModules(process, modules, sizeof(modules), &needed)) {
            size_t module_count = needed / sizeof(HMODULE);

            for (size_t i = 0; i < module_count; i++) {
                char module_name[MAX_PATH];
                MODULEINFO mod_info;

                if (GetModuleFileNameExA(process, modules[i], module_name, sizeof(module_name))) {
                    if (GetModuleInformation(process, modules[i], &mod_info, sizeof(mod_info))) {
                        ss << format_hex((DWORD64)mod_info.lpBaseOfDll) << " - "
                           << format_hex((DWORD64)mod_info.lpBaseOfDll + mod_info.SizeOfImage)
                           << "  " << module_name << "\n";
                    }
                }
            }
        } else {
            ss << "Failed to enumerate modules\n";
        }

        return ss.str();
    }

    //Get memory dump around crash address
    static std::string get_memory_dump(void* address) {
        std::stringstream ss;

        if (!address) {
            return "Invalid address (NULL)";
        }

        ss << "Memory dump at " << format_hex((DWORD64)address) << ":\n\n";

        // Try to read 128 bytes before and after the crash address
        const size_t dump_size = 128;
        BYTE* base_addr = (BYTE*)address - dump_size;

        for (int offset = -((int)dump_size); offset < (int)dump_size; offset += 16) {
            BYTE* addr = (BYTE*)address + offset;
            ss << format_hex((DWORD64)addr) << ": ";

            // Try to read 16 bytes
            BYTE buffer[16];
            SIZE_T bytes_read = 0;
            bool readable = ReadProcessMemory(GetCurrentProcess(), addr, buffer, 16, &bytes_read) != 0;

            if (readable && bytes_read > 0) {
                // Hex dump
                for (size_t i = 0; i < 16; i++) {
                    if (i < bytes_read) {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
                    } else {
                        ss << "?? ";
                    }
                }

                ss << " | ";

                // ASCII representation
                for (size_t i = 0; i < bytes_read; i++) {
                    char c = buffer[i];
                    ss << (isprint(c) ? c : '.');
                }
            } else {
                ss << "?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? | ????????????????";
            }

            // Mark the crash address line
            if (offset == 0) {
                ss << "  <-- CRASH HERE";
            }

            ss << "\n";
        }

        return ss.str();
    }

    //various callbacks needed to get into the crash handler during a crash (borrowed from backward.cpp)
    static inline void signal_handler(int signal) {
        crash_signal = signal;
        crash_handler();
        std::quick_exit(1);
    }
    static inline void terminator() {
        crash_handler();
        std::quick_exit(1);
    }
    __declspec(noinline) static LONG WINAPI exception_handler(EXCEPTION_POINTERS* ex_ptrs) {
        exception_pointers = ex_ptrs;
        crash_handler();
        return EXCEPTION_CONTINUE_SEARCH;
    }
    static void __cdecl invalid_parameter_handler(const wchar_t*,const wchar_t*,const wchar_t*,unsigned int,uintptr_t) {
        crash_handler();
        abort();
    }

    //callback needed during a normal exit to shut down the thread
    static inline void normal_exit() {
        status = program_status::normal_exit;
        cv.notify_one();
        if(output_thread.joinable())
            output_thread.join();
    }

    void end_session() {
        try {
            SetUnhandledExceptionFilter(nullptr);
            std::signal(SIGABRT, nullptr);
            std::signal(SIGSEGV, nullptr);
            std::signal(SIGILL, nullptr);
            std::set_terminate(nullptr);
            _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
            _set_purecall_handler(nullptr);
            _set_invalid_parameter_handler(nullptr);

            status = program_status::end_session;
            cv.notify_all();

            if (output_thread.joinable()) {
                output_thread.join();
            }
        } catch (...) {
            std::cerr << "Exception occurred during end_session" << std::endl;
            throw;
        }
    }

    //set up all the callbacks needed to get into the crash handler during a crash (borrowed from backward.cpp)
    void begin_monitoring() {
        // Initialize crash telemetry and user action logging
        CrashTelemetry::initialize();
        UserActionLogger::initialize();

        output_thread = std::thread(crash_handler_thread);

        SetUnhandledExceptionFilter(exception_handler);
        std::signal(SIGABRT, signal_handler);
        std::signal(SIGSEGV, signal_handler);
        std::signal(SIGILL, signal_handler);
        std::set_terminate(terminator);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _set_purecall_handler(terminator);
        _set_invalid_parameter_handler(&invalid_parameter_handler);

        std::atexit(normal_exit);
    }
}