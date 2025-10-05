#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include <lib/json/json_fwd.hpp>


class Setting {
public:
    virtual ~Setting() = default;

    [[nodiscard]] virtual nlohmann::json ToJson() const = 0;

    virtual void FromJson(const nlohmann::json &jsonData) = 0;

    [[nodiscard]] virtual std::unique_ptr<Setting> clone() const = 0;
};

template<typename T>
class SettingType : public Setting {
public:
    SettingType(std::string name, T defaultValue)
        : name(std::move(name)), value(std::move(defaultValue)) {
    }

    [[nodiscard]] nlohmann::json ToJson() const override;
    void FromJson(const nlohmann::json &jsonData) override;

    [[nodiscard]] std::unique_ptr<Setting> clone() const override {
        return std::make_unique<SettingType<T> >(name, value);
    }

    std::string name;
    T value;
};

class Settings {
public:
    template<typename T>
    void addSetting(const std::string &name, const T &defaultValue) {
        settings.emplace(name, std::make_unique<SettingType<T> >(name, defaultValue));
    }

    void deleteSetting(const std::string &name);

    void reset() {
        settings.clear();
    }

    void copyFrom(const Settings &from) {
        for (const auto &settingPair: from.settings) {
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

    template<typename FromType, typename ToType>
    void changeType(const std::string &name) {
        auto it = settings.find(name);
        if (it == settings.end()) return;

        auto *originalSetting = dynamic_cast<SettingType<FromType> *>(it->second.get());
        if (!originalSetting) return;

        ToType convertedValue;
        try {
            convertedValue = static_cast<ToType>(originalSetting->value);
        } catch (...) {
            return;
        }

        auto newSetting = std::make_unique<SettingType<ToType> >(name, convertedValue);

        settings[name] = std::move(newSetting);
    }

    void renameSetting(std::string oldName, std::string newName, bool clickguiColor = false) {
        if (clickguiColor) {
            renameSetting(oldName, "o_" + oldName, oldName + "_rgb", newName);
            return;
        }
        auto check = settings.find(newName);
        if (check != settings.end()) return;

        auto it = settings.find(oldName);
        if (it != settings.end()) {
            settings[newName] = std::move(it->second);
            settings.erase(it);
        }
    }

    void renameSetting(std::string oldCol, std::string oldOpacity, std::string oldRGB, std::string newName) {
        renameSetting(oldCol, newName + "Col");
        renameSetting(oldOpacity, newName + "Opacity");
        renameSetting(oldRGB, newName + "RGB");
    }

    template<typename T>
    SettingType<T> *getOrAddSettingByName(const std::string &name, const T &defaultValue) {
        auto it = settings.find(name);
        if (it != settings.end()) return static_cast<SettingType<T> *>(it->second.get());
        addSetting(name, defaultValue);

        return getSettingByName<T>(name);
    }

    template<typename T>
    void setValue(const std::string &name, const T &value) {
        auto setting = getSettingByName<T>(name);
        if (setting) {
            setting->value = value;
        }
    }

    [[nodiscard]] std::string ToJson() const;
    void AppendFromJson(const std::string &jsonString, bool old = false);
    void FromJson(const std::string &jsonString, bool old = false);

    std::unordered_map<std::string, std::unique_ptr<Setting> > settings;
};
