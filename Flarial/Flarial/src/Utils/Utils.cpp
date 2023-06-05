#include "Utils.h"

std::string Utils::getRoamingPath() {

    char* path = NULL;
    size_t length;

    _dupenv_s(&path, &length, "appdata");

    if (path == NULL)
        return "";

    return std::string(path) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState";

};