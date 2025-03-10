#pragma once

#include "ScriptSettingTypes.hpp"

#include <lua.hpp>
#include <Utils/Logger/Logger.hpp>
#include "Scripting/FlarialScript.hpp"

class ScriptSettingManager {
public:
    template<typename T, typename... Args>
    T* addSetting(const FlarialScript* script, const std::string& settingName, const std::string& settingDescription, Args&&... args) {

        auto& scriptSettings = settings[script];
        if (scriptSettings.contains(settingName)) {
            return dynamic_cast<T*>(scriptSettings[settingName].get());
        }

        auto setting = std::make_unique<T>(settingName, settingDescription, std::forward<Args>(args)...);
        T* ptr = setting.get();
        scriptSettings.emplace(settingName, std::move(setting));
        return ptr;
    }

    template<typename T>
    T* getSetting(const FlarialScript* script, const std::string& settingName) {
        auto it = settings.find(script);
        if (it != settings.end()) {
            auto sIt = it->second.find(settingName);
            if (sIt != it->second.end()) {
                return dynamic_cast<T*>(sIt->second.get());
            }
        }
        return nullptr;
    }

    void loadSettings(FlarialScript* script);
    void saveSettings(const FlarialScript* script);

    void clearSettingsForScript(const FlarialScript* script) {
        settings.erase(script);
    }

    const auto& getAllSettings() const { return settings; }
private:
    std::unordered_map<const FlarialScript*, std::unordered_map<std::string, std::unique_ptr<BaseSetting>>> settings;
};
