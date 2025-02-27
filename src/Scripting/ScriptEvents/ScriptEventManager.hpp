#pragma once

#include <lua.hpp>

#include <Utils/Logger/Logger.hpp>

class ScriptEventManager {
public:
    static void registerEventAPI(lua_State* L);
    static void clearAll();

    /**
     * @brief Triggers a named event with N arguments.
     * @param eventName The name for the event that will be used in Lua.
     * @param args The args for the callback that can be used in Lua.
     * @return True if the event is cancelled.
     */
    template <typename... Args>
    static bool triggerEvent(const std::string& eventName, Args&&... args) {
        const auto it = mListeners.find(eventName);
        if (it == mListeners.end()) return false; // No one is listening
        
        // For every (luaState, funcRef) callback, call the function
        bool canceled = false;
        for (auto& [luaState, ref] : it->second) {
            lua_rawgeti(luaState, LUA_REGISTRYINDEX, ref);
            
            auto pushArg = [&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_integral_v<T>) {
                    lua_pushinteger(luaState, (lua_Integer)val);
                } else if constexpr (std::is_floating_point_v<T>) {
                    lua_pushnumber(luaState, (lua_Number)val);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    lua_pushstring(luaState, val.c_str());
                } else if constexpr (std::is_same_v<T, const char*>) {
                    lua_pushstring(luaState, val);
                } else {
                    lua_pushnil(luaState);
                }
            };
            
            (pushArg(args), ...);
            
            int argCount = sizeof...(args);
            if (lua_pcall(luaState, argCount, 1, 0) != LUA_OK) {
                Logger::error("Error calling event listener for {}: {}", eventName, lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                continue;
            }
            
            if (lua_isboolean(luaState, -1)) {
                bool ret = (lua_toboolean(luaState, -1) != 0);
                if (ret) {
                    canceled = true;
                }
            }
            lua_pop(luaState, 1);
            
            if (canceled) {
                break;
            }
        }
        return canceled;
    }

private:
    static std::unordered_map<std::string, std::vector<std::pair<lua_State*, int>>> mListeners;
    static void registerListener(lua_State* L, const std::string& eventName);
    static int lua_event_listen(lua_State* L);
};
