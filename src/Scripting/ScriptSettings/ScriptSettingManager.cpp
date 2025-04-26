#include "ScriptSettingManager.hpp"

#include <Scripting/ScriptManager.hpp>
#include <Client/Module/Modules/Module.hpp>

void ScriptSettingManager::loadSettings(Script* script) {
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
                module->settings.addSetting<std::string>(name, keyBindSetting->defaultValue);
            }

            auto* keybindValue = module->settings.getSettingByName<std::string>(name);
            if (keybindValue) {
                keyBindSetting->value = keybindValue->value;
            } else {
                keyBindSetting->value = keyBindSetting->defaultValue;
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void ScriptSettingManager::saveSettings(const Script* script) {
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

            module->settings.getSettingByName<std::string>(name)->value = keybindSetting->value;
            break;
        }
        default: {
            break;
        }
        }
    }
}

HeaderSetting* ScriptSettingManager::addHeader(const Script* script, const std::string& text) {
    auto& scriptSettings = settings[script];

    std::string headerKey = "__header_" + std::to_string(scriptSettings.size());

    auto setting = std::make_unique<HeaderSetting>(text);
    HeaderSetting* ptr = setting.get();
    scriptSettings.emplace(headerKey, std::move(setting));
    insertionOrder[script].push_back(headerKey);
    return ptr;
}

ExtraPaddingSetting* ScriptSettingManager::extraPadding(const Script* script) {
    auto& scriptSettings = settings[script];

    std::string paddingKey = "__padding_" + std::to_string(scriptSettings.size());

    auto setting = std::make_unique<ExtraPaddingSetting>();
    ExtraPaddingSetting* ptr = setting.get();
    scriptSettings.emplace(paddingKey, std::move(setting));
    insertionOrder[script].push_back(paddingKey);
    return ptr;
}

ButtonSetting* ScriptSettingManager::addButton(const Script* script, const std::string& name, const std::string& description, const std::string& buttonText, std::function<void()> action) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<ButtonSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<ButtonSetting>(name, description, buttonText, std::move(action));
    ButtonSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    insertionOrder[script].push_back(name);
    return ptr;
}

TextBoxSetting* ScriptSettingManager::addTextBox(const Script* script, const std::string& name, const std::string& description, const std::string& defaultValue, int limit) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<TextBoxSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<TextBoxSetting>(name, description, defaultValue, limit);
    TextBoxSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    insertionOrder[script].push_back(name);
    return ptr;
}

KeybindSetting* ScriptSettingManager::addKeybind(const Script* script, const std::string& name, const std::string& description, const std::string& defaultKey) {
    auto& scriptSettings = settings[script];

    if (scriptSettings.contains(name)) {
        return dynamic_cast<KeybindSetting*>(scriptSettings[name].get());
    }

    auto setting = std::make_unique<KeybindSetting>(name, description, defaultKey);
    KeybindSetting* ptr = setting.get();
    scriptSettings.emplace(name, std::move(setting));
    insertionOrder[script].push_back(name);
    return ptr;
}