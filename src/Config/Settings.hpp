#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>
#include "json/json.hpp"
#include "../Utils/Logger/Logger.hpp"

using json = nlohmann::json;

class Setting {
public:
    virtual ~Setting() = default;
    virtual json ToJson() const = 0;
    virtual void FromJson(const json& jsonData) = 0;
    virtual std::unique_ptr<Setting> clone() const = 0;
};

template<typename T>
class SettingType : public Setting {
public:
    SettingType(std::string name, const T& defaultValue) : value(defaultValue), name(name) {}

    json ToJson() const override {
        json jsonData;
        jsonData["name"] = name;
        jsonData["value"] = value;
        return jsonData;
    }

    void FromJson(const json& jsonData) override {
        if (jsonData.is_object() && jsonData.contains("name") && jsonData.contains("value")) {
            name = jsonData["name"].get<std::string>();
            value = jsonData["value"].get<T>();
        }
    }

    std::unique_ptr<Setting> clone() const override {
        return std::make_unique<SettingType<T>>(name, value);
    }

    std::string name;
    T value;
};

class Settings {
public:
    template<typename T>
    void addSetting(const std::string& name, const T& defaultValue) {
        settings[name] = std::make_unique<SettingType<T>>(name, defaultValue);
    }

    void reset() {
        settings.clear();
    }

    void copyFrom(const Settings& from) {

        for (const auto& settingPair : from.settings) {
            const std::string& name = settingPair.first;
            const std::unique_ptr<Setting>& sourceSetting = settingPair.second;


            settings[name] = sourceSetting->clone();
        }

    }


    template<typename T>
    SettingType<T>* getSettingByName(const std::string& name) {

        auto it = settings.find(name);
        if (it != settings.end()) {
            return static_cast<SettingType<T>*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    void setValue(const std::string& name, const T& value) {
        auto setting = getSettingByName<T>(name);
        if (setting) {
            setting->value = value;
        }
    }

    std::string ToJson() const {
        json jsonData;
        for (const auto& settingPair : settings) {
            jsonData.push_back(settingPair.second->ToJson());
        }
        return jsonData.dump(4);
    }

    void FromJson(const std::string& jsonString) {
        try {
            json jsonData = json::parse(jsonString);
            for (const auto& item : jsonData) {
                std::string name = item["name"].get<std::string>();

                if (item["value"].is_number()) {
                    float value = item["value"].get<float>();
                    addSetting(name, value);
                } else if (item["value"].is_string()) {
                    std::string value = item["value"].get<std::string>();
                    addSetting(name, value);
                } else if (item["value"].is_boolean()) {
                    bool value = item["value"].get<bool>();
                    addSetting(name, value);
                } else if (item["value"].is_null()) {
                    // Handle null value if needed
                } else {
                    // Handle unsupported value type
                }
            }
        } catch (const std::exception& e) {

            if(!jsonString.empty())
                Logger::error(e.what());
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Setting>> settings;
};
