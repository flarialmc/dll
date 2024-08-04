#pragma once

#include <string>

#define STR(x) #x
#define STRINGIFY(x) STR(x)

#define CheckHRESULTNoErr(HR, OP) \
    if (FAILED(hr = HR)) { \
        Logger::error(" failed, with HRESULT 0x%lx", hr); \
    }
#define CheckHRESULTEx(HR, OP) \
    if (FAILED(hr = HR)) { \
        Logger::error(" failed, with HRESULT 0x%lx", hr); \
    }
#define CheckHRESULT(HR) CheckHRESULTEx(HR, "Operation")


class Logger {
public:
    static std::string file;
    static FILE *fp;
private:
    static void writeToFile(const std::string&);

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
