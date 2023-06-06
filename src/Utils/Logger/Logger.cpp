#include "Logger.hpp"
#include "../Utils.hpp"
#include <Windows.h>
#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>

std::string Logger::file = Utils::getRoamingPath() + "\\Flarial\\logs\\latest.log";

void Logger::writeToFile(std::string str)
{
    if (std::filesystem::exists(file))
    {
        std::filesystem::path p(file);
        std::filesystem::create_directories(p.parent_path().string());
    }

    CloseHandle(CreateFileA(file.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));

    if (str.length() > 0)
    {
        std::ofstream fileOutput;
        fileOutput.open(file.c_str(), std::ios_base::app);
        fileOutput << str << std::endl;
        fileOutput.close();
    }
}

void Logger::initialize()
{
    if (std::filesystem::exists(file))
    {
        std::filesystem::remove(file);
    }
}

void Logger::debug(std::string str)
{
    std::string log = std::format("[DEBUG]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}

void Logger::info(std::string str)
{
    std::string log = std::format("[INFO]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}

void Logger::warn(std::string str)
{
    std::string log = std::format("[WARN]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}

void Logger::error(std::string str)
{
    std::string log = std::format("[ERROR]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}

void Logger::fatal(std::string str)
{
    std::string log = std::format("[FATAL]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}

void Logger::trace(std::string str)
{
    std::string log = std::format("[TRACE]: {}", str);
    std::cout << str << std::endl;
    writeToFile(log);
}
