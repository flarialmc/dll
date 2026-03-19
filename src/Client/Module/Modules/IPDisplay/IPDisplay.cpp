#include "IPDisplay.hpp"

void IPDisplay::customConfig()
{
    setDef("responsivewidth", true);
    setDef("textscale", 0.80f);
    setDef("port", false);
}

void IPDisplay::customSettings()
{
    addToggle("Show Port", "", "port");
}

std::string IPDisplay::getDisplayValue()
{
    std::string IPStr = SDK::getServerIP();
    std::string port = SDK::getServerPort();
    std::string fullstr = IPStr;
    if (getOps<bool>("port")) fullstr += ":" + port;
    return fullstr;
}
