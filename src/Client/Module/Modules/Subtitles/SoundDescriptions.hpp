#pragma once

#include <string>
#include <unordered_map>

class SoundDescriptions {

private:
    static const std::unordered_map<std::string, std::string> soundDescriptions;

public:
    static std::string getSoundDescription(const std::string& soundName);
    static std::string getSoundSimpleDescription(const std::string& soundName);

};
