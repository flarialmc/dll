#include "Logger.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>
#include "crashlogs.hpp"
#include "LightweightCrashTracker.hpp"
#include <filesystem>
#include <fstream>


namespace Logger {
    void writeToFile(const std::string& message) {
        try {
            const std::string path = Utils::getClientPath() + "\\logs\\latest.log";

            std::ofstream file(path, std::ios::app);
            if (!file.is_open()) {
                throw std::ios_base::failure("Failed to open latest.log");
            }

            file << message << std::endl;

            file.close();
        }
        catch (const std::exception& e) {
            std::cerr << "Error writing to log file: " << e.what() << std::endl;
        }
    }

    void initialize() {
        const std::string path = Utils::getClientPath() + "\\logs\\latest.log";

        glaiel::crashlogs::set_crashlog_folder(Utils::getLogsPath());
        glaiel::crashlogs::begin_monitoring();

        // Initialize lightweight crash tracking
        CrashTracker::initialize();
        
        if (std::filesystem::exists(path)) {
            std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }

#if defined(__DEBUG__)
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

        auto message = fmt::format("{}\n{}\n{}\n{}\n{}\n\n",
            fmt::format("Client Version: {}", FLARIAL_VERSION),
            fmt::format("Game Version:   {}", WinrtUtils::getFormattedVersion()),
            fmt::format("Commit:         {}", COMMIT_HASH),
            fmt::format("Build Type:     {}", FLARIAL_BUILD_TYPE),
            fmt::format("Build Date:     {}", FLARIAL_BUILD_DATE)
        );
        writeToFile(message);
    }

    void shutdown() {
        bool close = true; // Make this true if u want console to close on eject

        if (close) {
            ShowWindow(GetConsoleWindow(), SW_HIDE);
            system("cls");
        } else {
            FreeConsole();
        }
    }

    void printColors() {
    struct ColorInfo {
        const char* name;
        fmt::color color;
    };

    ColorInfo colors[] = {
        {"alice_blue", fmt::color::alice_blue},
        {"antique_white", fmt::color::antique_white},
        {"aqua", fmt::color::aqua},
        {"aquamarine", fmt::color::aquamarine},
        {"azure", fmt::color::azure},
        {"beige", fmt::color::beige},
        {"bisque", fmt::color::bisque},
        {"black", fmt::color::black},
        {"blanched_almond", fmt::color::blanched_almond},
        {"blue", fmt::color::blue},
        {"blue_violet", fmt::color::blue_violet},
        {"brown", fmt::color::brown},
        {"burly_wood", fmt::color::burly_wood},
        {"cadet_blue", fmt::color::cadet_blue},
        {"chartreuse", fmt::color::chartreuse},
        {"chocolate", fmt::color::chocolate},
        {"coral", fmt::color::coral},
        {"cornflower_blue", fmt::color::cornflower_blue},
        {"cornsilk", fmt::color::cornsilk},
        {"crimson", fmt::color::crimson},
        {"cyan", fmt::color::cyan},
        {"dark_blue", fmt::color::dark_blue},
        {"dark_cyan", fmt::color::dark_cyan},
        {"dark_golden_rod", fmt::color::dark_golden_rod},
        {"dark_gray", fmt::color::dark_gray},
        {"dark_green", fmt::color::dark_green},
        {"dark_khaki", fmt::color::dark_khaki},
        {"dark_magenta", fmt::color::dark_magenta},
        {"dark_olive_green", fmt::color::dark_olive_green},
        {"dark_orange", fmt::color::dark_orange},
        {"dark_orchid", fmt::color::dark_orchid},
        {"dark_red", fmt::color::dark_red},
        {"dark_salmon", fmt::color::dark_salmon},
        {"dark_sea_green", fmt::color::dark_sea_green},
        {"dark_slate_blue", fmt::color::dark_slate_blue},
        {"dark_slate_gray", fmt::color::dark_slate_gray},
        {"dark_turquoise", fmt::color::dark_turquoise},
        {"dark_violet", fmt::color::dark_violet},
        {"deep_pink", fmt::color::deep_pink},
        {"deep_sky_blue", fmt::color::deep_sky_blue},
        {"dim_gray", fmt::color::dim_gray},
        {"dodger_blue", fmt::color::dodger_blue},
        {"fire_brick", fmt::color::fire_brick},
        {"floral_white", fmt::color::floral_white},
        {"forest_green", fmt::color::forest_green},
        {"fuchsia", fmt::color::fuchsia},
        {"gainsboro", fmt::color::gainsboro},
        {"ghost_white", fmt::color::ghost_white},
        {"gold", fmt::color::gold},
        {"golden_rod", fmt::color::golden_rod},
        {"gray", fmt::color::gray},
        {"green", fmt::color::green},
        {"green_yellow", fmt::color::green_yellow},
        {"honey_dew", fmt::color::honey_dew},
        {"hot_pink", fmt::color::hot_pink},
        {"indian_red", fmt::color::indian_red},
        {"indigo", fmt::color::indigo},
        {"ivory", fmt::color::ivory},
        {"khaki", fmt::color::khaki},
        {"lavender", fmt::color::lavender},
        {"lavender_blush", fmt::color::lavender_blush},
        {"lawn_green", fmt::color::lawn_green},
        {"lemon_chiffon", fmt::color::lemon_chiffon},
        {"light_blue", fmt::color::light_blue},
        {"light_coral", fmt::color::light_coral},
        {"light_cyan", fmt::color::light_cyan},
        {"light_golden_rod_yellow", fmt::color::light_golden_rod_yellow},
        {"light_gray", fmt::color::light_gray},
        {"light_green", fmt::color::light_green},
        {"light_pink", fmt::color::light_pink},
        {"light_salmon", fmt::color::light_salmon},
        {"light_sea_green", fmt::color::light_sea_green},
        {"light_sky_blue", fmt::color::light_sky_blue},
        {"light_slate_gray", fmt::color::light_slate_gray},
        {"light_steel_blue", fmt::color::light_steel_blue},
        {"light_yellow", fmt::color::light_yellow},
        {"lime", fmt::color::lime},
        {"lime_green", fmt::color::lime_green},
        {"linen", fmt::color::linen},
        {"magenta", fmt::color::magenta},
        {"maroon", fmt::color::maroon},
        {"medium_aquamarine", fmt::color::medium_aquamarine},
        {"medium_blue", fmt::color::medium_blue},
        {"medium_orchid", fmt::color::medium_orchid},
        {"medium_purple", fmt::color::medium_purple},
        {"medium_sea_green", fmt::color::medium_sea_green},
        {"medium_slate_blue", fmt::color::medium_slate_blue},
        {"medium_spring_green", fmt::color::medium_spring_green},
        {"medium_turquoise", fmt::color::medium_turquoise},
        {"medium_violet_red", fmt::color::medium_violet_red},
        {"midnight_blue", fmt::color::midnight_blue},
        {"mint_cream", fmt::color::mint_cream},
        {"misty_rose", fmt::color::misty_rose},
        {"moccasin", fmt::color::moccasin},
        {"navajo_white", fmt::color::navajo_white},
        {"navy", fmt::color::navy},
        {"old_lace", fmt::color::old_lace},
        {"olive", fmt::color::olive},
        {"olive_drab", fmt::color::olive_drab},
        {"orange", fmt::color::orange},
        {"orange_red", fmt::color::orange_red},
        {"orchid", fmt::color::orchid},
        {"pale_golden_rod", fmt::color::pale_golden_rod},
        {"pale_green", fmt::color::pale_green},
        {"pale_turquoise", fmt::color::pale_turquoise},
        {"pale_violet_red", fmt::color::pale_violet_red},
        {"papaya_whip", fmt::color::papaya_whip},
        {"peach_puff", fmt::color::peach_puff},
        {"peru", fmt::color::peru},
        {"pink", fmt::color::pink},
        {"plum", fmt::color::plum},
        {"powder_blue", fmt::color::powder_blue},
        {"purple", fmt::color::purple},
        {"rebecca_purple", fmt::color::rebecca_purple},
        {"red", fmt::color::red},
        {"rosy_brown", fmt::color::rosy_brown},
        {"royal_blue", fmt::color::royal_blue},
        {"saddle_brown", fmt::color::saddle_brown},
        {"salmon", fmt::color::salmon},
        {"sandy_brown", fmt::color::sandy_brown},
        {"sea_green", fmt::color::sea_green},
        {"sea_shell", fmt::color::sea_shell},
        {"sienna", fmt::color::sienna},
        {"silver", fmt::color::silver},
        {"sky_blue", fmt::color::sky_blue},
        {"slate_blue", fmt::color::slate_blue},
        {"slate_gray", fmt::color::slate_gray},
        {"snow", fmt::color::snow},
        {"spring_green", fmt::color::spring_green},
        {"steel_blue", fmt::color::steel_blue},
        {"tan", fmt::color::tan},
        {"teal", fmt::color::teal},
        {"thistle", fmt::color::thistle},
        {"tomato", fmt::color::tomato},
        {"turquoise", fmt::color::turquoise},
        {"violet", fmt::color::violet},
        {"wheat", fmt::color::wheat},
        {"white", fmt::color::white},
        {"white_smoke", fmt::color::white_smoke},
        {"yellow", fmt::color::yellow},
        {"yellow_green", fmt::color::yellow_green}
    };

    for (const auto& color : colors) {
        fmt::print(fg(color.color), "fmt::color::{}\n", color.name);
    }
}
}