#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "json/json.hpp"
#include "../Utils/Logger/Logger.hpp"

using json = nlohmann::json;

// Forward declaration
class Setting;

// Customizable Setting type
template<typename T>
class SettingType {
public:
    SettingType(const std::string& name, const T& defaultValue) : name(name), value(defaultValue) {}

    std::string name;
    T value;
};

// Base Setting class
class Setting {
public:
    virtual ~Setting() = default;
    virtual json ToJson() const = 0;
    virtual void FromJson(const json& jsonData) = 0;
};

// Template specialization for SettingType<T>
template<typename T>
class TypedSetting : public Setting {
public:
    TypedSetting(const std::string& name, const T& defaultValue) : setting(name, defaultValue) {}

    json ToJson() const override {
        json jsonData;
        jsonData["name"] = setting.name;
        jsonData["value"] = setting.value;
        return jsonData;
    }

    void FromJson(const json& jsonData) override {
        if (jsonData.is_object() && jsonData.contains("name") && jsonData.contains("value")) {
            setting.name = jsonData["name"].get<std::string>();
            setting.value = jsonData["value"].get<T>();
        }
    }

    SettingType<T> setting;
};

// Settings container
class Settings {
public:
    template<typename T>
    void addSetting(const std::string& name, const T& defaultValue) {
        settings.emplace_back(new TypedSetting<T>(name, defaultValue));
    }

    template<typename T>
    SettingType<T>* getSettingByName(const std::string& name) {

        for (auto& setting : settings) {
            auto typedSetting = dynamic_cast<TypedSetting<T>*>(setting.get());
            if (typedSetting && typedSetting->setting.name == name) {
                return &(typedSetting->setting);
            }
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
        for (const auto& setting : settings) {
            jsonData.push_back(setting->ToJson());
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
            Logger::error(e.what());
        }
    }

private:
    std::vector<std::unique_ptr<Setting>> settings;
};