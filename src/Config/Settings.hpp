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
};

template<typename T>
class SettingType : public Setting {
public:
    SettingType(const T& defaultValue) : value(defaultValue) {}

    json ToJson() const override {
        json jsonData;
        jsonData["value"] = value;
        return jsonData;
    }

    void FromJson(const json& jsonData) override {
        if (jsonData.contains("value")) {
            value = jsonData["value"].get<T>();
        }
    }

    T value;
};

class Settings {
public:
    template<typename T>
    void addSetting(const std::string& name, const T& defaultValue) {
        settings[name] = std::make_unique<SettingType<T>>(defaultValue);
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
            jsonData[settingPair.first] = settingPair.second->ToJson();
        }
        return jsonData.dump(4);
    }

    void FromJson(const std::string& jsonString) {
        try {
            json jsonData = json::parse(jsonString);
            for (const auto& item : jsonData.items()) {
                std::string name = item.key();
                const json& valueData = item.value();

                if (valueData.is_object()) {
                    if (valueData.contains("value")) {
                        const json& value = valueData["value"];
                        if (value.is_number()) {
                            float floatValue = value.get<float>();
                            addSetting(name, floatValue);
                        } else if (value.is_string()) {
                            std::string stringValue = value.get<std::string>();
                            addSetting(name, stringValue);
                        } else if (value.is_boolean()) {
                            bool boolValue = value.get<bool>();
                            addSetting(name, boolValue);
                        } else if (value.is_null()) {
                            // Handle null value if needed
                        } else {
                            // Handle unsupported value type
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            if (!jsonString.empty()) {
                Logger::error(e.what());
            }
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Setting>> settings;
};
