#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("settings")
                .addFunction("addBool", [state](const std::string& name, const std::string& desc, bool defValue) -> BoolSetting* {
                    LuaRef scriptNameRef = getGlobal(state, "__scriptName");
                    LuaRef scriptName = LuaRef(state, "Unknown");

                    if (scriptNameRef.isString()) {
                        scriptName = scriptNameRef.cast<std::string>();
                    }

                    auto* setting = gScriptSettingManager.addSetting<BoolSetting>(
                        scriptName.cast<std::string>().value(), name, desc, defValue
                    );
                    return setting;
                })
            .endNamespace();
    }
};