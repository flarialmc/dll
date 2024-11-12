#include "Logger.hpp"

#include "../Utils.hpp"
#include "../Versions/WinrtUtils.hpp"
#include "crashlogs.hpp"
#include <format>
#include <filesystem>
#include <fstream>


namespace Logger {
    namespace Color {
        fmt::color success() { return fmt::color::forest_green;  }
        fmt::color debug() { return fmt::color::medium_sea_green; }
        fmt::color info() { return fmt::color::steel_blue; }
        fmt::color warn() { return fmt::color::orange; }
        fmt::color error() { return fmt::color::indian_red; }
        fmt::color fatal() { return fmt::color::medium_purple; }
    }

    void writeToFile(const std::string& message) {
        try {
            const std::string path = Utils::getRoamingPath() + "\\Flarial\\latest.log";

            std::ofstream file(path, std::ios::app);
            if (!file.is_open()) {
                throw std::ios_base::failure("Failed to open logs.txt");
            }

            file << message << std::endl;

            file.close();
        }
        catch (const std::exception& e) {
            std::cerr << "Error writing to log file: " << e.what() << std::endl;
        }
    }

    void initialize() {
        const std::string path = Utils::getRoamingPath() + "\\Flarial\\latest.log";

        glaiel::crashlogs::set_crashlog_folder(Utils::getRoamingPath() + "\\Flarial\\Logs");
        glaiel::crashlogs::begin_monitoring();

        if (std::filesystem::exists(path)) {
            std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }

#ifndef NDEBUG
        HWND console = GetConsoleWindow();
        if (!console) {
            AllocConsole();
            SetConsoleTitleA("Flarial-Debugger");
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
        else {
            ShowWindow(console, SW_SHOW);
        }
#endif

        auto message = fmt::format("{}\n{}\n{}\n{}\n\n",
            fmt::format("===================="),
            fmt::format("Game Version: {}", WinrtUtils::getVersion()),
            fmt::format("Commit: {}", COMMIT_HASH),
            fmt::format("====================")
        );
        writeToFile(message);
    }

    void shutdown() {
        FreeConsole();
        // This closes the console but hides useful eject logs
        /*
        ShowWindow(GetConsoleWindow(), SW_HIDE);
        system("cls");
        */
    }
}