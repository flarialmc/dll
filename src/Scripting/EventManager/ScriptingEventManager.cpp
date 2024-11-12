#include "ScriptingEventManager.hpp"
#include <type_traits>


std::unordered_map<std::string, std::vector<ScriptingEventManager::EventEntry>> ScriptingEventManager::eventHandlers;

void ScriptingEventManager::registerHandler(lua_State* L, const std::string& eventName) {
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    eventHandlers[eventName].push_back({L, ref});
}

void pushAny(lua_State* L, const std::any& arg) {
    if (arg.type() == typeid(int)) {
        lua_pushinteger(L, std::any_cast<int>(arg));
    } else if (arg.type() == typeid(double)) {
        lua_pushnumber(L, std::any_cast<double>(arg));
    } else if (arg.type() == typeid(std::string)) {
        lua_pushstring(L, std::any_cast<std::string>(arg).c_str());
    } else if (arg.type() == typeid(bool)) {
        lua_pushboolean(L, std::any_cast<bool>(arg));
    } else if (arg.type() == typeid(const char*)) {
        lua_pushstring(L, std::any_cast<const char*>(arg));
    } else {
        lua_pushnil(L);
    }
}

void ScriptingEventManager::triggerEvent(lua_State* L, const std::string& eventName, const std::vector<std::any>& args) {
    auto it = eventHandlers.find(eventName);
    if (it == eventHandlers.end()) return;

    for (const auto& handler : it->second) {
        if (handler.luaState != L){
            continue;
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, handler.ref);

        for (const auto& arg : args) {
            pushAny(L, arg);
        }

        if (lua_pcall(L, args.size(), 0, 0) != LUA_OK) {
            std::string err = lua_tostring(L, -1);
            Logger::error("Lua error in event handler: " + err);
            lua_pop(L, 1);
        }
    }
}

void ScriptingEventManager::clearHandlers(lua_State* L) {
    for (auto& [eventName, handlers] : eventHandlers) {
        auto it = handlers.begin();
        while (it != handlers.end()) {
            if (it->luaState == L) {
                luaL_unref(L, LUA_REGISTRYINDEX, it->ref);
                it = handlers.erase(it);
            } else {
                ++it;
            }
        }
    }
}