#pragma once

#include <string>
#include <unordered_map>

class SoundDescriptions {
public:
    static std::string getSoundDescription(const std::string& soundName);

private:
    static const std::unordered_map<std::string, std::string> soundDescriptions;
};
