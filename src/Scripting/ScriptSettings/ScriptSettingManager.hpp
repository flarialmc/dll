#pragma once

#include "ScriptSettingTypes.hpp"

#include <lua.hpp>
#include <Utils/Logger/Logger.hpp>
#include "Scripting/Script.hpp"

class ScriptSettingManager {
public:
    template<typename T, typename... Args>
    T* addSetting(const Script* script, const std::string& settingName, const std::string& settingDescription, Args&&... args) {

        auto& scriptSettings = settings[script];
        if (scriptSettings.contains(settingName)) {
            return dynamic_cast<T*>(scriptSettings[settingName].get());
        }

        auto setting = std::make_unique<T>(settingName, settingDescription, std::forward<Args>(args)...);
        T* ptr = setting.get();
        scriptSettings.emplace(settingName, std::move(setting));
        insertionOrder[script].push_back(settingName);
        return ptr;
    }

    template<typename T>
    T* getSetting(const Script* script, const std::string& settingName) {
        auto it = settings.find(script);
        if (it != settings.end()) {
            auto sIt = it->second.find(settingName);
            if (sIt != it->second.end()) {
                return dynamic_cast<T*>(sIt->second.get());
            }
        }
        return nullptr;
    }

    void loadSettings(Script* script);
    void saveSettings(const Script* script);

    void clearSettingsForScript(const Script* script) {
        settings.erase(script);
        insertionOrder.erase(script);
    }

    HeaderSetting* addHeader(const Script* script, const std::string& text);
    ExtraPaddingSetting* extraPadding(const Script* script);
    ButtonSetting* addButton(const Script* script, const std::string& name, const std::string& description, const std::string& buttonText, std::function<void()> action);
    TextBoxSetting* addTextBox(const Script* script, const std::string& name, const std::string& description, const std::string& defaultValue, int limit);
    KeybindSetting* addKeybind(const Script* script, const std::string& name, const std::string& description, const std::string& defaultKey);

    const auto& getAllSettings() const { return settings; }

    const std::vector<std::string>& getInsertionOrder(const Script* script) const {
        static const std::vector<std::string> empty;
        auto it = insertionOrder.find(script);
        return it != insertionOrder.end() ? it->second : empty;
    }
private:
    std::unordered_map<const Script*, std::unordered_map<std::string, std::unique_ptr<BaseSetting>>> settings;
    std::unordered_map<const Script*, std::vector<std::string>> insertionOrder;
};