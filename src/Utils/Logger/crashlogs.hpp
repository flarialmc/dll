//CrashLogs
//by Tyler Glaiel, 2023
//simple C++ crashlog output library based on C++23's <stacktrace> header
//call glaiel::crashlogs::begin_monitoring() at the start of your program
//and it will save a stack trace to a crash log if the program crashes!
//a decent amount of this was copied & modified from backward.cpp (https://github.com/bombela/backward-cpp)
//but with <stacktrace> it prints nice stack traces without needing to include pdbs and is also significantly simpler

#pragma once
#include <string>

namespace glaiel::crashlogs {
    //Begins crash monitoring. Crash logs will not be generated until begin_monitoring has been called
    void begin_monitoring();

    //set the folder path that crashlogs will be saved in.
    //if the folder doesn't exist, it will be created when the program crashes
    void set_crashlog_folder(std::string folderpath);

    //sets the format to use for crash log filenames. Default is "crash_{timestamp}.txt". Crashes are saved in the crashlog folder
    void set_crashlog_filename(std::string filename_format);

    //after a crash log is written, this optional callback is called with the full file path of where the crash log was written
    void set_on_write_crashlog_callback(void(*on_write_crashlog)(std::string));

    //the most recently set crashlog header will be printed at the top of the crash log file.
    void set_crashlog_header_message(std::string message);
    std::string get_crashlog_header_message();
}