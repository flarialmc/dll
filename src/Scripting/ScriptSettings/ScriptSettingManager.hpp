#pragma once

#include "ScriptSettingTypes.hpp"

#include <lua.hpp>
#include <Utils/Logger/Logger.hpp>

class ScriptSettingManager {
public:
    template<typename T, typename... Args>
    T* addSetting(const std::string& scriptName, const std::string& settingName, const std::string& settingDescription, Args&&... args) {
        std::string uniqueKey = scriptName + "." + settingName;

        if (settings.find(uniqueKey) != settings.end()) {
            Logger::warn("Setting '{}' already exists, returning existing value.", uniqueKey);
            return dynamic_cast<T*>(settings[uniqueKey].get());
        }

        auto setting = std::make_unique<T>(settingName, uniqueKey, settingDescription, std::forward<Args>(args)...);
        Logger::info("Creating new setting '{}' with default value: {}", uniqueKey, setting->defaultValue);

        T* ptr = setting.get();
        settings.emplace(uniqueKey, std::move(setting));
        return ptr;
    }

    template<typename T>
    T* getSettingByName(const std::string& name) {
        auto it = settings.find(name);
        if (it != settings.end())
            return dynamic_cast<T*>(it->second.get());
        return nullptr;
    }

    void clearSettingsForModule(const std::string& module) {
        std::erase_if(settings, [&module](const auto& pair) {
            return pair.first.starts_with(module + ".");
        });
    }

    [[nodiscard]] const auto& getAllSettings() const { return settings; }
private:
    std::unordered_map<std::string, std::unique_ptr<BaseSetting>> settings;
};
