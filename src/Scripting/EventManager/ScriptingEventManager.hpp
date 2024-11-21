#pragma once

#include <lua.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <any>

class ScriptingEventManager {
public:
    struct EventEntry {
        lua_State* luaState;
        int ref;
    };
    static std::unordered_map<int, std::vector<EventEntry>> eventHandlers;
    static void registerHandler(lua_State* L, const int& eventName);
    static void triggerEvent(lua_State* L, const int eventName, const std::vector<std::any>& args);
    static void clearHandlers();
};
