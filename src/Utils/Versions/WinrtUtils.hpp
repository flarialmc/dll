#pragma once

// Made by marioCST, ported by FreezeEngine

#include <string>

struct Version {
    int major = 0;
    int minor = 0;
    int build = 0;
    std::string error;
};

class WinrtUtils {
public:
    static Version getMCVersion();
    static bool check(int m, int b);
    static std::string getVersion();
    static std::string getFormattedVersion();
};
