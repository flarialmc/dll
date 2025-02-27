#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            const char* name = luaL_checkstring(L, 1);
            const char* desc = luaL_checkstring(L, 2);
            bool defValue = lua_toboolean(L, 3);

            lua_getglobal(L, "__scriptName");
            std::string scriptName = "Unknown";
            if (lua_isstring(L, -1)) {
                scriptName = lua_tostring(L, -1);
            }
            lua_pop(L, 1);

            auto* ptr = gScriptSettingManager.addSetting<BoolSetting>(
                scriptName,
                name,
                desc,
                defValue
            );

            auto** ud = static_cast<BoolSetting**>(lua_newuserdata(L, sizeof(BoolSetting*)));
            *ud = ptr;

            luaL_setmetatable(L, "BoolSettingMeta");
            lua_setmetatable(L, -2);

            return 1;
        }, "addBool", "settings");
    }
};