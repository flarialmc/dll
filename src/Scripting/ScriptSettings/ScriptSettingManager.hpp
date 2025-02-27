#pragma once

#include "ScriptSettingTypes.hpp"

#include <lua.hpp>

class ScriptSettingManager {
public:
    template<typename T, typename... Args>
    T* addSetting(const std::string& scriptName, const std::string& settingName, const std::string& settingDescription, Args&&... args) {
        std::string uniqueKey = scriptName + "." + settingName;
        auto setting = std::make_unique<T>(uniqueKey, settingDescription, std::forward<Args>(args)...);
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

    [[nodiscard]] const auto& getAllSettings() const { return settings; }

    static void createBoolSetting(lua_State* L);

private:
    std::unordered_map<std::string, std::unique_ptr<BaseSetting>> settings;
};
