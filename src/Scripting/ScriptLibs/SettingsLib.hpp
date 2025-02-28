#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;
/*
        getGlobalNamespace(state)
            .beginNamespace("settings")
                .addFunction("addBool", [state](const std::string& name, const std::string& desc, bool defValue) -> BoolSetting* {
                    LuaRef scriptNameRef = getGlobal(state, "__scriptName");
                    std::string scriptName = "Unknown";

                    if (scriptNameRef.isString()) {
                        scriptName = scriptNameRef.cast<std::string>();
                    }

                    auto* BoolSetting = gScriptSettingManager.addSetting<BoolSetting>(scriptName, name, desc, defValue);

                    return BoolSetting;
                })
            .endNamespace();
     */
    }
};