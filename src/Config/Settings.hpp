#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <lib/json/json.hpp>
#include <Utils/Logger/Logger.hpp>

using json = nlohmann::json;

class Setting {
public:
    virtual ~Setting() = default;

    [[nodiscard]] virtual json ToJson() const = 0;

    virtual void FromJson(const json &jsonData) = 0;

    [[nodiscard]] virtual std::unique_ptr<Setting> clone() const = 0;
};

template<typename T>
class SettingType : public Setting {
public:
    SettingType(std::string name, T defaultValue)
        : name(std::move(name)), value(std::move(defaultValue)) {}

    [[nodiscard]] json ToJson() const override {
        return json{{"name", name}, {"value", value}};
    }

    void FromJson(const json &jsonData) override {
        if (jsonData.is_object() && jsonData.contains("name") && jsonData.contains("value")) {
            name = jsonData["name"].get<std::string>();
            value = jsonData["value"].get<T>();
        }
    }

    [[nodiscard]] std::unique_ptr<Setting> clone() const override {
        return std::make_unique<SettingType<T>>(name, value);
    }

    std::string name;
    T value;
};

class Settings {
public:
    template<typename T>
    void addSetting(const std::string &name, const T &defaultValue) {
        settings.emplace(name, std::make_unique<SettingType<T>>(name, defaultValue));
    }

    void deleteSetting(const std::string &name) {
        auto it = settings.find(name);
        if (it != settings.end()) {
            settings.erase(it);
        } else {
            Logger::error("Setting not found: {}", name);
        }
    }

    void reset() {
        settings.clear();
    }

    void copyFrom(const Settings &from) {
        for (const auto &settingPair : from.settings) {
            settings[settingPair.first] = settingPair.second->clone();
        }
    }

    template<typename T>
    SettingType<T> *getSettingByName(const std::string &name) {
        auto it = settings.find(name);
        if (it != settings.end()) {
            return static_cast<SettingType<T> *>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    void setValue(const std::string &name, const T &value) {
        auto setting = getSettingByName<T>(name);
        if (setting) {
            setting->value = value;
        }
    }

    [[nodiscard]] std::string ToJson() const {
        json jsonData;
        for (const auto &settingPair : settings) {
            jsonData.push_back(settingPair.second->ToJson());
        }
        return jsonData.dump(4);
    }

    void FromJson(const std::string &jsonString) {
        if (jsonString.empty()) {
            Logger::error("JSON string is empty");
            return;
        }

        try {
            json jsonData = json::parse(jsonString);
            std::unordered_map<std::string, std::unique_ptr<Setting>> newSettings;

            for (const auto &item : jsonData) {
                std::string name = item["name"].get<std::string>();

                if (item["value"].is_number_float()) {
                    newSettings.emplace(name, std::make_unique<SettingType<float>>(name, item["value"].get<float>()));
                } else if (item["value"].is_string()) {
                    newSettings.emplace(name, std::make_unique<SettingType<std::string>>(name, item["value"].get<std::string>()));
                } else if (item["value"].is_boolean()) {
                    newSettings.emplace(name, std::make_unique<SettingType<bool>>(name, item["value"].get<bool>()));
                } else {
                    Logger::warn("Unsupported or null value type for setting '{}'", name);
                }
            }

            settings = std::move(newSettings);

        } catch (const json::parse_error &e) {
            Logger::error("An error occured while parsing settings: {}", e.what());
        }
    }

    std::unordered_map<std::string, std::unique_ptr<Setting>> settings;
};
