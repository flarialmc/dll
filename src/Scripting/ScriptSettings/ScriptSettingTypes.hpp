#pragma once

#include <lua.hpp>
#include <utility>
#include <LuaBridge/LuaBridge.h>

enum class ScriptSettingType {
    Toggle,
    Button,
    Slider,
    TextBox,
    Keybind,
};

struct BaseSetting {
    std::string name;
    std::string description;
    ScriptSettingType type;
    virtual ~BaseSetting() = default;
};

struct ToggleSetting : BaseSetting {
    bool value;
    bool defaultValue;

    ToggleSetting(const std::string& name, const std::string& description, const bool def)
    : value(def), defaultValue(def) {
        this->name = name;
        this->description = description;
        this->type = ScriptSettingType::Toggle;
    }
};

struct ButtonSetting : BaseSetting {
    std::string buttonText;
    std::function<void()> action;

    ButtonSetting(const std::string& name, const std::string& description, std::string buttonText, std::function<void()> action)
        : buttonText(std::move(buttonText)), action(std::move(action)) {
        this->name = name;
        this->description = description;
        this->type = ScriptSettingType::Button;
    }
};

struct SliderSetting : BaseSetting {
    float value;
    float defaultValue;
    float minValue;
    float maxValue;
    bool zerosafe;

    SliderSetting(const std::string& name, const std::string& description, const float defaultVal, const float maxVal, const float minVal, bool zerosafe = true)
        : value(defaultVal), defaultValue(defaultVal), maxValue(maxVal), minValue(minVal), zerosafe(zerosafe) {
        this->name = name;
        this->description = description;
        this->type = ScriptSettingType::Slider;
    }
};

struct TextBoxSetting : BaseSetting {
    std::string value;
    std::string defaultValue;
    int characterLimit;

    TextBoxSetting(const std::string& name, const std::string& description, std::string defaultValue, const int limit)
        : value(std::move(defaultValue)), defaultValue(defaultValue), characterLimit(limit) {
        this->name = name;
        this->description = description;
        this->type = ScriptSettingType::TextBox;
    }
};

struct KeybindSetting : BaseSetting {
    std::string value;
    std::string defaultValue;

    KeybindSetting(const std::string& name, const std::string& description, std::string defaultValue)
        : value(std::move(defaultValue)), defaultValue(std::move(defaultValue)) {
        this->name = name;
        this->description = description;
        this->type = ScriptSettingType::Keybind;
    }
};