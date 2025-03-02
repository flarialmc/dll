#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<BoolSetting>("BoolSetting")
                .addProperty("value", &BoolSetting::defaultValue)
            .endClass();

        getGlobalNamespace(state)
            .beginNamespace("settings")
                .addFunction("addBool", [state](const std::string& name, const std::string& desc, bool defValue) -> BoolSetting* {
                    lua_getglobal(state, "name");
                    std::string scriptName = "Unknown";

                    if (lua_isstring(state, -1)) {
                        scriptName = lua_tostring(state, -1);
                    }
                    lua_pop(state, 1);

                    auto* setting = gScriptSettingManager.addSetting<BoolSetting>(
                        scriptName, name, desc, defValue
                    );
                    return setting;
                })
            .endNamespace();
    }
};