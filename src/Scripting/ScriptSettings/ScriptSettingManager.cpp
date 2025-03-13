#include "ScriptSettingManager.hpp"

#include <Scripting/ScriptManager.hpp>

void ScriptSettingManager::loadSettings(FlarialScript* script) {
    if (!script) return;

    auto* module = ScriptManager::getModuleByState(script->getState());
    if (!module) return;

    for (auto& [name, setting] : settings[script]) {
        switch (setting->type) {
            case ScriptSettingType::Toggle: {
                auto* toggleSetting = dynamic_cast<ToggleSetting*>(setting.get());
                if (!toggleSetting) return;

                if (module->settings.getSettingByName<bool>(name) == nullptr) {
                    module->settings.addSetting<bool>(name, toggleSetting->defaultValue);
                }

                auto* toggleValue = module->settings.getSettingByName<bool>(name);
                if (toggleValue) {
                    toggleSetting->value = toggleValue->value;
                } else {
                    toggleSetting->value = toggleSetting->defaultValue;
                }
                break;
            }
            case ScriptSettingType::Slider: {
                auto* sliderSetting = dynamic_cast<SliderSetting*>(setting.get());
                if (!sliderSetting) return;

                if (module->settings.getSettingByName<float>(name) == nullptr) {
                    module->settings.addSetting<float>(name, sliderSetting->defaultValue);
                }

                auto* sliderValue = module->settings.getSettingByName<float>(name);
                if (sliderValue) {
                    sliderSetting->value = sliderValue->value;
                } else {
                    sliderSetting->value = sliderSetting->defaultValue;
                }
                break;
            }
            case ScriptSettingType::TextBox: {
                auto* textBoxSetting = dynamic_cast<TextBoxSetting*>(setting.get());
                if (!textBoxSetting) return;

                if (module->settings.getSettingByName<std::string>(name) == nullptr) {
                    module->settings.addSetting<std::string>(name, textBoxSetting->defaultValue);
                }

                auto* textBoxValue = module->settings.getSettingByName<std::string>(name);
                if (textBoxValue) {
                    textBoxSetting->value = textBoxValue->value;
                } else {
                    textBoxSetting->value = textBoxSetting->defaultValue;
                }
                break;
            }
            case ScriptSettingType::Keybind: {
                auto* keyBindSetting = dynamic_cast<KeybindSetting*>(setting.get());
                if (!keyBindSetting) return;

                if (module->settings.getSettingByName<std::string>(name) == nullptr) {
                    module->settings.addSetting<std::string>(name, keyBindSetting->defaultKeybind);
                }

                auto* textBoxValue = module->settings.getSettingByName<std::string>(name);
                if (textBoxValue) {
                    keyBindSetting->keybind = textBoxValue->value;
                } else {
                    keyBindSetting->keybind = keyBindSetting->defaultKeybind;
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void ScriptSettingManager::saveSettings(const FlarialScript* script) {
    if (!script) return;

    auto* module = ScriptManager::getModuleByState(script->getState());
    if (!module) return;

    for (const auto& [name, setting] : settings[script]) {
        switch (setting->type) {
            case ScriptSettingType::Toggle: {
                auto* boolSetting = dynamic_cast<ToggleSetting*>(setting.get());
                if (!boolSetting) return;

                module->settings.getSettingByName<bool>(name)->value = boolSetting->value;
                break;
            }
            case ScriptSettingType::Slider: {
                auto* sliderSetting = dynamic_cast<SliderSetting*>(setting.get());
                if (!sliderSetting) return;

                module->settings.getSettingByName<float>(name)->value = sliderSetting->value;
                break;
            }
            case ScriptSettingType::TextBox: {
                auto* textBoxSetting = dynamic_cast<TextBoxSetting*>(setting.get());
                if (!textBoxSetting) return;

                module->settings.getSettingByName<std::string>(name)->value = textBoxSetting->value;
                break;
            }
            case ScriptSettingType::Keybind: {
                auto* keybindSetting = dynamic_cast<KeybindSetting*>(setting.get());
                if (!keybindSetting) return;

                module->settings.getSettingByName<std::string>(name)->value = keybindSetting->keybind;
                break;
            }
            default: {
                break;
            }
        }
    }
}

ButtonSetting* ScriptSettingManager::addButton(const FlarialScript* script, const std::string& name, const std::string& description, const std::string& buttonText, std::function<void()> action) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<ButtonSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<ButtonSetting>(name, description, buttonText, std::move(action));
    ButtonSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    return ptr;
}

TextBoxSetting* ScriptSettingManager::addTextBox(const FlarialScript* script, const std::string& name, const std::string& description, const std::string& defaultValue, int limit) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<TextBoxSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<TextBoxSetting>(name, description, defaultValue, limit);
    TextBoxSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    return ptr;
}

KeybindSetting* ScriptSettingManager::addKeybind(const FlarialScript* script, const std::string& name, const std::string& description, const std::string& defaultKey) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<KeybindSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<KeybindSetting>(name, description, defaultKey);
    KeybindSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    return ptr;
}