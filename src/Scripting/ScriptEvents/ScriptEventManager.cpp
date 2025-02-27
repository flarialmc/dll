#include "ScriptEventManager.hpp"

std::unordered_map<std::string, std::vector<std::pair<lua_State*, int>>> ScriptEventManager::mListeners;

void ScriptEventManager::registerEventAPI(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, lua_event_listen);
    lua_setfield(L, -2, "listen");
    lua_setglobal(L, "event");
}

void ScriptEventManager::clearAll() {
    for (auto& [name, vec] : mListeners) {
        for (auto& [luaState, funcRef] : vec) {
            luaL_unref(luaState, LUA_REGISTRYINDEX, funcRef);
        }
        vec.clear();
    }
    mListeners.clear();
}

int ScriptEventManager::lua_event_listen(lua_State* L) {
    // Arg 1 = event name
    // Arg 2 = function
    const char* eventName = luaL_checkstring(L, 1);

    if (!lua_isfunction(L, 2)) {
        return luaL_error(L, "Second argument must be a function");
    }
    // Copy that function up so we can store it
    lua_pushvalue(L, 2);

    // Register it
    registerListener(L, eventName);

    return 0;
}

void ScriptEventManager::registerListener(lua_State* L, const std::string& eventName) {
    // The function to store is on top of stack
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    mListeners[eventName].emplace_back(L, ref);
}