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

    template <typename... Args>
    static bool triggerEvent(lua_State* L, int eventName, const Args&... args);

    static void clearHandlers();




};
