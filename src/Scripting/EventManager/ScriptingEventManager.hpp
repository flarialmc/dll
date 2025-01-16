#pragma once

#include <lua.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <any>

class ScriptingEventManager {
public:
    static std::unordered_map<lua_State*, std::unordered_map<int, int>> eventHandlers;
    static void registerHandler(lua_State* L, const int& eventName);

    template <typename... Args>
    static bool triggerEvent(lua_State* L, int eventName, const Args&... args);

    static void clearHandlers();




};
