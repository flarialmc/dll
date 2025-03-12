#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;


        getGlobalNamespace(state)
            .beginClass<BoolSetting>("BoolSetting")
                .addProperty("value", &BoolSetting::value)
            .endClass();

        getGlobalNamespace(state)
            .beginNamespace("settings")
                .addFunction("addBool", [state](const std::string& name, const std::string& desc, bool defValue) -> BoolSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return nullptr;

                    auto* setting = gScriptSettingManager.addSetting<BoolSetting>(script, name, desc, defValue);
                    return setting;
                })
            .endNamespace();
    }
};