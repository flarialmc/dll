#include "crashlogs.hpp"

#include <Client/Module/Manager.hpp>
#include <kiero/kiero.h>

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

//needed for being able to get into the crash handler on a crash
#include <csignal>
#include <exception>
#include <cstdlib>
#define WIN32_LEAN_AND_MEAN

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

    static std::filesystem::path get_log_filepath();
    static const char* try_get_signal_name(int signal);

    //output the crashlog file after a crash has occured
    static void output_crash_log() {
        std::filesystem::path path = get_log_filepath();
        std::ofstream log(path);

        log << "COMMIT_HASH: " << COMMIT_HASH << std::endl;

        if(!header_message.empty()) log << header_message << std::endl;
        if(crash_signal != 0) {
            log << "Received signal " << crash_signal << " " << try_get_signal_name(crash_signal) << std::endl;
        }
        log << trace << "\n\n";
        log << "Enabled modules:" << "\n";

        for (const auto& pair : ModuleManager::moduleMap) {
            if(pair.second->isEnabled())
                log << pair.second->name << "\n";
        }

        log.close();

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
    __declspec(noinline) static LONG WINAPI exception_handler(EXCEPTION_POINTERS*) {
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