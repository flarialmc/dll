#pragma once
#include <string>

class Logger
{
public:
    static std::string file;
    static FILE* fp;
private:
    static void writeToFile(std::string);

public:
    static void initialize();

public:
    static void debug(std::string);
    static void info(std::string);
    static void warn(std::string);
    static void error(std::string);
    static void fatal(std::string);
    static void trace(std::string);
};
