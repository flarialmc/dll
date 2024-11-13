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

namespace Logger {
    namespace Color {
        fmt::color success();
        fmt::color debug();
        fmt::color info();
        fmt::color warn();
        fmt::color error();
        fmt::color fatal();
    }

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
        Logger::print(fg(Color::success()), "SUCCESS", fmt, args...);
    }

    void debug(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(Color::debug()), "DEBUG", fmt, args...);
    }

    void info(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(Color::info()), "INFO", fmt, args...);
    }

    void warn(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(Color::warn()), "WARN", fmt, args...);
    }

    void error(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(Color::error()), "ERROR", fmt, args...);
    }

    void fatal(const std::string& fmt, auto&& ... args) {
        Logger::print(fg(Color::fatal()), "FATAL", fmt, args...);
    }

    void custom(const fmt::text_style& ts, const std::string& level, const std::string& fmt, auto&& ... args) {
        Logger::print(ts, level, fmt, args...);
    }
};
