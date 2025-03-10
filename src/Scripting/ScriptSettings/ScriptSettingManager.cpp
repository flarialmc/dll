#include "ScriptSettingManager.hpp"

#include <Scripting/ScriptManager.hpp>

void ScriptSettingManager::loadSettings(FlarialScript* script) {
    if (!script) return;

    auto* module = ScriptManager::getModuleByState(script->getState());
    if (!module) {
        Logger::error("No module for loading");
        return;
    }

    for (auto& [name, setting] : settings[script]) {
        switch (setting->type) {
            case ScriptSettingType::Bool: {
                auto* boolSetting = dynamic_cast<BoolSetting*>(setting.get());
                if (boolSetting) {

                    if (module->settings.getSettingByName<bool>(name) == nullptr) {
                        Logger::info("no value");
                        module->settings.addSetting<bool>(name, boolSetting->defaultValue);
                    }

                    Logger::info("updated value");
                    boolSetting->value = module->settings.getSettingByName<bool>(name);
                }
                break;
            }
        }
    }
}

void ScriptSettingManager::saveSettings(const FlarialScript* script) {
    if (!script) return;

    auto* module = ScriptManager::getModuleByState(script->getState());
    if (!module) {
        Logger::error("No module for saving");
        return;
    }

    for (const auto& [name, setting] : settings[script]) {
        switch (setting->type) {
            case ScriptSettingType::Bool: {
                auto* boolSetting = dynamic_cast<BoolSetting*>(setting.get());
                if (boolSetting) {
                    Logger::info("Setting bool value");
                    module->settings.setValue<bool>(name, boolSetting->value);
                }
                break;
            }
        }
    }
}