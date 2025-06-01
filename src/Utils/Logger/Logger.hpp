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

#define LOG_ERROR(fmt, ...) Logger::error(fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::fatal(fmt, __FILE__, __LINE__, ##__VA_ARGS__)


namespace Logger {
    void writeToFile(const std::string& string);
    void initialize();
    void shutdown();
    void printColors();

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

    void error(const std::string& fmt, std::string f = __FILE__, int l = __LINE__, auto&& ... args) {
        std::string n = fmt; n.insert(0, "[at {}:{}] ");
        Logger::print(fg(fmt::color::crimson), "ERROR", n, f, l, args...);
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
