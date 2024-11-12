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
    static std::unordered_map<std::string, std::vector<EventEntry>> eventHandlers;
    static void registerHandler(lua_State* L, const std::string& eventName);
    static void triggerEvent(lua_State* L, const std::string& eventName, const std::vector<std::any>& args);
    static void clearHandlers(lua_State* L);
};
