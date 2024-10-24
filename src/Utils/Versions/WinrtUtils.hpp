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
    static bool checkAboveOrEqual(int m, int b);
    static bool checkBelowOrEqual(int m, int b);
    static bool checkBetween(int m1, int b1, int m2, int b2);
    static std::string getVersion();
    static std::string getFormattedVersion();
};
