#include "Utils.h"

std::string Utils::getRoamingPath()
{
    char* path = nullptr;
    size_t length;

    _dupenv_s(&path, &length, "appdata");

    if (path == nullptr)
        return "";

    return std::string(path) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState";
};
