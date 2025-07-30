#include "OptionsParser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../../Utils/Logger/Logger.hpp"
#include <Utils/WinrtUtils.hpp>
#include <Utils/Utils.hpp>

std::map<std::string, std::string> OptionsParser::parseOptionsFile() {

    std::filesystem::path path((winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\games\\com.mojang\\minecraftpe\\options.txt").c_str());
    std::ifstream file(path);
    
    if (!file.is_open()) {
        LOG_ERROR("Failed to open options.txt");
        return OptionsParser::options;
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
