#include <string>

enum SettingType {
    Bool = 1,
    Float = 2,
    Keybind = 3,
    Color = 4,
    Vector2 = 5
};

class BaseSetting {
public:
    SettingType type;
    std::string name;
    std::string tooltip;
    bool visible;
};

class BooleanSetting : public BaseSetting {
public:
    bool* status = nullptr;
public:
    BooleanSetting(std::string name, std::string tooltip, bool visible, bool* var) {
        this->type = SettingType::Bool;
        this->name = name;
        this->tooltip = tooltip;
        this->visible = visible;
        this->status = var;
    }
};

class FloatSetting : public BaseSetting {
public:
    float min, max;
    float* current;
public:
    FloatSetting(std::string name, std::string tooltip, bool visible, float min, float max, float* current) {
        this->type = SettingType::Float;
        this->name = name;
        this->tooltip = tooltip;
        this->visible = visible;
        this->min = min;
        this->max = max;
        this->current = current;
    }
};

class KeybindSetting : public BaseSetting {
public:
    uintptr_t key;
public:
    KeybindSetting(std::string name, std::string tooltip, bool visible, uintptr_t key) {
        this->type = SettingType::Keybind;
        this->name = name;
        this->tooltip = tooltip;
        this->visible = visible;
        this->key = key;
    }
};

class ColorSetting : public BaseSetting {
public:
    float* color;
public:
    ColorSetting(std::string name, std::string tooltip, bool visible, float* color) {
        this->type = SettingType::Color;
        this->name = name;
        this->tooltip = tooltip;
        this->visible = visible;
        this->color = color;
    }
};

template <typename T>
class Vec2Setting : public BaseSetting {
public:
    Vec2<T>* vector;
public:
    Vec2Setting(std::string name, std::string tooltip, bool visible, Vec2<T>* vector) {
        this->type = SettingType::Vector2;
        this->name = name;
        this->tooltip = tooltip;
        this->visible = visible;
        this->vector = vector;
    }
};