#include "ScriptingEventManager.hpp"
#include "../../SDK/Client/Network/Packet/Packet.hpp"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include <type_traits>


std::unordered_map<int, std::vector<ScriptingEventManager::EventEntry>> ScriptingEventManager::eventHandlers;



template void ScriptingEventManager::triggerEvent<int, int>(lua_State* L, int eventName, const int& arg1, const int& arg2);
template void ScriptingEventManager::triggerEvent<Packet*, int>(lua_State* L, int eventName, Packet* const& arg1, const int& arg2);
template void ScriptingEventManager::triggerEvent<>(lua_State* L, int eventName);



void ScriptingEventManager::registerHandler(lua_State* L, const int& eventName) {
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    eventHandlers[eventName].push_back({L, ref});
}

template<typename T>
void pushToLua(lua_State* L, T arg) {
    if constexpr (std::is_same_v<T, int>) {
        lua_pushinteger(L, arg);
    } else if constexpr (std::is_same_v<T, double>) {
        lua_pushnumber(L, arg);
    } else if constexpr (std::is_same_v<T, std::string>) {
        lua_pushstring(L, arg.c_str());
    } else if constexpr (std::is_same_v<T, bool>) {
        lua_pushboolean(L, arg);
    } else if constexpr (std::is_same_v<T, const char*>) {
        lua_pushstring(L, arg);
    }
    else if constexpr (std::is_pointer_v<T>) {
        lua_pushlightuserdata(L, static_cast<void *>(arg));
    }
    else {
        lua_pushnil(L);
    }
}



template<typename... Args>
void ScriptingEventManager::triggerEvent(lua_State* L, int eventName, const Args&... args) {
    auto it = eventHandlers.find(eventName);
    if (it == eventHandlers.end()) return;

    for (const auto& handler : it->second) {
        if (handler.luaState != L) {
            continue;
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, handler.ref);

        (pushToLua(L, args), ...);

        if (lua_pcall(L, sizeof...(args), 0, 0) != LUA_OK) {
            std::string err = lua_tostring(L, -1);
            Logger::error("Error while executing event handler: {}", err);
            lua_pop(L, 1);
        }
    }
}


void ScriptingEventManager::clearHandlers() {
    for (auto &[eventName, handlers]: eventHandlers) {
        auto it = handlers.begin();
        while (it != handlers.end()) {
            luaL_unref(it->luaState, LUA_REGISTRYINDEX, it->ref);
            it = handlers.erase(it);

        }
    }
}