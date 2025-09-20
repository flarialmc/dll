#pragma once

#include <string>
#include <fmt/format.h>
#include <fmt/args.h>
#include <fmt/base.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/os.h>
#include <fmt/ranges.h>
#include <windows.h>  // For HRESULT

#define LOG_ERROR(fmt, ...) Logger::error(fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::fatal(fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_HRESULT_ERROR(msg, hr) Logger::error("{}: {}", msg, Logger::getHRESULTError(hr))


namespace Logger {
    void writeToFile(const std::string& string);
    void initialize();
    void shutdown();
    void printColors();
    
    // Helper function to format HRESULT errors
    inline std::string getHRESULTError(HRESULT hr) {
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
            (LPSTR)&messageBuffer, 0, NULL);
        
        std::string message = (size > 0 && messageBuffer) ? messageBuffer : "Unknown error";
        if (messageBuffer) LocalFree(messageBuffer);
        
        // Remove trailing newlines
        while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
            message.pop_back();
        }
        
        return fmt::format("0x{:08X} - {}", static_cast<unsigned long>(hr), message);
    }

    void print(const fmt::text_style& ts, const std::string& level, const std::string& fmt, auto&& ... args) {
        try {
            auto message = fmt::format(fmt::runtime(fmt), args...);

            auto fullMessage = fmt::format("{}{}{} {}",
                fmt::format(fmt::fg(fmt::color::gray), "["),
                fmt::format(ts, "{}", level),
                fmt::format(fmt::fg(fmt::color::gray), "]"),
                fmt::format(ts, "{}", message)
            );

            fmt::print("{}\n", fullMessage);
            fflush(stdout);

            writeToFile("[" + level + "] " + message);
        }
        catch (const std::exception& e) {
            fmt::print("Logging error: {}\n", e.what());
        }
    }

    void print(const std::string& fmt, auto&& ... args) {
        try {
            auto message = fmt::format(fmt::runtime(fmt), args...);
            fmt::print("{}", message);
            fflush(stdout);

            writeToFile(message);
        }
        catch (const std::exception& e) {
            fmt::print("Logging error: {}\n", e.what());
        }
    }

    void println(const std::string& fmt, auto&& ... args) {
        try {
            auto message = fmt::format(fmt::runtime(fmt), args...);
            fmt::print("{}\n", message);
            fflush(stdout);

            writeToFile(message);
        }
        catch (const std::exception& e) {
            fmt::print("Logging error: {}\n", e.what());
        }
    }

    void success(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(fmt::color::forest_green), "SUCCESS", fmt, args...);
    }

    void debug(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(fmt::color::orchid), "DEBUG", fmt, args...);
    }

    void info(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(fmt::color::light_steel_blue), "INFO", fmt, args...);
    }

    void warn(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(fmt::color::orange), "WARN", fmt, args...);
    }

    // Simple error without file/line info
    template<typename... Args>
    void error(const std::string& fmt, Args&&... args) {
        Logger::print(fg(fmt::color::crimson), "ERROR", fmt, std::forward<Args>(args)...);
    }
    
    // Error with file/line info (no default parameters to avoid ambiguity)
    template<typename... Args>
    void error(const std::string& fmt, const std::string& file, int line, Args&&... args) {
        std::string n = fmt; 
        n.insert(0, "[at {}:{}] ");
        Logger::print(fg(fmt::color::crimson), "ERROR", n, file, line, std::forward<Args>(args)...);
    }

    void fatal(const std::string& fmt, std::string f = __FILE__, int l = __LINE__, auto&& ... args) {
        std::string n = fmt; n.insert(0, "[at {}:{}] ");
        Logger::print(fg(fmt::color::rebecca_purple), "FATAL", n, f, l, args...);
    }

    void custom(const fmt::text_style& ts, const std::string& level, const std::string& fmt, auto&& ... args) {
        Logger::print(ts, level, fmt, args...);
    }

    void script(const bool error, const std::string& fmt, auto&& ... args) {
        if (!error) {
            Logger::print(fg(fmt::color::aqua), "SCRIPT", fmt, args...);
        } else {
            Logger::print(fg(fmt::color::crimson), "SCRIPT ERROR", fmt, args...);
        }
    }
};
