#pragma once
#include <string>

enum MouseAction { None = 0, Left = 1, Right = 2, Middle = 3, Scroll = 4 };

class Utils
{
public:
    static std::string getRoamingPath();
};
