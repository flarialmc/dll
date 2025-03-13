#include "ScriptSettingManager.hpp"

#include <Scripting/ScriptManager.hpp>

void ScriptSettingManager::loadSettings(FlarialScript* script) {
    if (!script) return;

    auto* module = ScriptManager::getModuleByState(script->getState());
    if (!module) return;

    for (auto& [name, setting] : settings[script]) {
        switch (setting->type) {
            case ScriptSettingType::Bool: {
                auto* boolSetting = dynamic_cast<BoolSetting*>(setting.get());
                if (!boolSetting) return;

                if (module->settings.getSettingByName<bool>(name) == nullptr) {
                    module->settings.addSetting<bool>(name, boolSetting->defaultValue);
                }

                auto* boolValue = module->settings.getSettingByName<bool>(name);
                if (boolValue) {
                    boolSetting->value = boolValue->value;
                } else {
                    boolSetting->value = boolSetting->defaultValue;
                }
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
            case ScriptSettingType::Bool: {

                auto* boolSetting = dynamic_cast<BoolSetting*>(setting.get());
                if (!boolSetting) return;

                module->settings.getSettingByName<bool>(name)->value = boolSetting->value;
                std::cout << module->settings.getSettingByName<bool>(name)->value << std::endl;
                break;
            }
            default: ;
        }
    }
}