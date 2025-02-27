#include "ScriptSettingManager.hpp"

int boolSettingIndex(lua_State* L) {
    auto** ud = static_cast<BoolSetting**>(
        luaL_checkudata(L, 1, "BoolSettingMeta")
    );
    BoolSetting* setting = *ud;

    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "value") == 0) {
        lua_pushboolean(L, setting->value);
        return 1;
    }
    return 0;
}

int boolSettingNewIndex(lua_State* L) {
    auto** ud = static_cast<BoolSetting**>(
        luaL_checkudata(L, 1, "BoolSettingMeta")
    );
    BoolSetting* setting = *ud;

    const char* key = luaL_checkstring(L, 2);
    if (strcmp(key, "value") == 0) {
        bool val = lua_toboolean(L, 3);
        setting->value = val;
        return 0;
    }
    return 0;
}

void ScriptSettingManager::createBoolSetting(lua_State* L) {
    luaL_newmetatable(L, "BoolSettingMeta");

    lua_pushcfunction(L, boolSettingIndex);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, boolSettingNewIndex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
}