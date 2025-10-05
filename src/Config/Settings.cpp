#include "Settings.hpp"
#include <lib/json/json.hpp>
#include <Utils/Logger/Logger.hpp>

using json = nlohmann::json;

// SettingType template implementations
template<typename T>
nlohmann::json SettingType<T>::ToJson() const {
    return value;
}

template<typename T>
void SettingType<T>::FromJson(const nlohmann::json &jsonData) {
    if (!jsonData.is_primitive()) {
        Logger::error("Invalid JSON format for setting '{}'", name);
        return;
    }
    value = jsonData.get<T>();
}

// Explicit template instantiations for common types
template class SettingType<bool>;
template class SettingType<int>;
template class SettingType<float>;
template class SettingType<std::string>;

// Settings implementations
void Settings::deleteSetting(const std::string &name) {
    auto it = settings.find(name);
    if (it != settings.end()) {
        settings.erase(it);
    } else {
        Logger::error("Setting not found: {}", name);
    }
}

std::string Settings::ToJson() const {
    json jsonData;
    for (const auto &settingPair: settings) {
        jsonData[settingPair.first] = settingPair.second->ToJson();
    }
    auto dump = jsonData.dump(4);
    return dump;
}

void Settings::AppendFromJson(const std::string &jsonString, bool old) {
    if (jsonString.empty()) {
        Logger::error("JSON string is empty");
        return;
    }

    try {
        json jsonData = json::parse(jsonString);

        if (!old && !jsonData.is_object()) return;

        if (old)
            for (const auto &item: jsonData) {
                std::string name = item["name"].get<std::string>();
                if (item["value"].is_number_float()) this->settings.emplace(name, std::make_unique<SettingType<float> >(name, item["value"].get<float>()));
                else if (item["value"].is_number_integer()) this->settings.emplace(name, std::make_unique<SettingType<int> >(name, item["value"].get<int>()));
                else if (item["value"].is_string()) this->settings.emplace(name, std::make_unique<SettingType<std::string> >(name, item["value"].get<std::string>()));
                else if (item["value"].is_boolean()) this->settings.emplace(name, std::make_unique<SettingType<bool> >(name, item["value"].get<bool>()));
                else Logger::warn("Unsupported or null value type for setting '{}'", name);
            }
        else
            for (const auto &[key, value]: jsonData.items()) {
                if (value.is_boolean()) this->settings[key] = std::make_unique<SettingType<bool> >(key, value.get<bool>());
                else if (value.is_number_integer()) this->settings[key] = std::make_unique<SettingType<int> >(key, value.get<int>());
                else if (value.is_number_float()) this->settings[key] = std::make_unique<SettingType<float> >(key, value.get<float>());
                else if (value.is_string()) this->settings[key] = std::make_unique<SettingType<std::string> >(key, value.get<std::string>());
                else Logger::warn("Unsupported JSON value type for setting '{}' during append. Skipping.", key);
            }
    } catch (const json::parse_error &e) {
        Logger::error("An error occurred while parsing settings: {}", e.what());
    }
}

void Settings::FromJson(const std::string &jsonString, bool old) {
    if (jsonString.empty()) {
        Logger::error("JSON string is empty");
        return;
    }

    try {
        json jsonData = json::parse(jsonString);

        if (!old && !jsonData.is_object()) return;

        std::unordered_map<std::string, std::unique_ptr<Setting> > newSettings;

        if (old)
            for (const auto &item: jsonData) {
                std::string name = item["name"].get<std::string>();
                if (item["value"].is_number_float()) newSettings.emplace(name, std::make_unique<SettingType<float> >(name, item["value"].get<float>()));
                else if (item["value"].is_number_integer()) newSettings.emplace(name, std::make_unique<SettingType<int> >(name, item["value"].get<int>()));
                else if (item["value"].is_string()) newSettings.emplace(name, std::make_unique<SettingType<std::string> >(name, item["value"].get<std::string>()));
                else if (item["value"].is_boolean()) newSettings.emplace(name, std::make_unique<SettingType<bool> >(name, item["value"].get<bool>()));
                else Logger::warn("Unsupported or null value type for setting '{}'", name);
            }
        else
            for (const auto &[key, value]: jsonData.items()) {
                if (value.is_boolean()) newSettings[key] = std::make_unique<SettingType<bool> >(key, value.get<bool>());
                else if (value.is_number_float()) newSettings[key] = std::make_unique<SettingType<float> >(key, value.get<float>());
                else if (value.is_number_integer()) newSettings[key] = std::make_unique<SettingType<int> >(key, value.get<int>());
                else if (value.is_string()) newSettings[key] = std::make_unique<SettingType<std::string> >(key, value.get<std::string>());
                else Logger::warn("Unsupported value type for setting '{}'", key);
            }

        settings = std::move(newSettings);
    } catch (const json::parse_error &e) {
        Logger::error("An error occurred while parsing settings: {}", e.what());
    }
}
