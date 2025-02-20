#include "OptionsParser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../../Utils/Logger/Logger.hpp"
#include <Utils/WinrtUtils.hpp>

std::map<std::string, std::string> OptionsParser::parseOptionsFile() {
    std::ifstream file(winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()) + "\\games\\com.mojang\\minecraftpe\\options.txt");
    
    if (!file) {
        Logger::error("Failed to open options.txt");
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            OptionsParser::options[key] = value;
        }
    }
    file.close();
    return OptionsParser::options;
}
