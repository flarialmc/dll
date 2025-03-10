#pragma once
#include "Scripting/FlarialScript.hpp"

enum class ScriptSettingType {
    Bool,
    String,
    Int,
    Float,
    TextBox,
    KeyBind,
};

struct BaseSetting {
    std::string name;
    std::string description;
    ScriptSettingType type;
    virtual ~BaseSetting() = default;
};

struct BoolSetting : BaseSetting {
    bool value;
    bool defaultValue;
    std::string displayName;

    BoolSetting(const std::string& displayName, const std::string& description, bool def)
        : value(def), defaultValue(def), displayName(displayName) {
        this->description = description;
        this->type = ScriptSettingType::Bool;
    }
};