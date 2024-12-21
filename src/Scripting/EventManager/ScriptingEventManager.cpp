#include "ScriptingEventManager.hpp"
#include "../../SDK/Client/Network/Packet/Packet.hpp"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include <type_traits>
#include <Scripting/LUAHelper.hpp>


std::unordered_map<int, std::vector<ScriptingEventManager::EventEntry>> ScriptingEventManager::eventHandlers;



template bool ScriptingEventManager::triggerEvent<int, int>(lua_State* L, int eventName, const int& arg1, const int& arg2);
template bool ScriptingEventManager::triggerEvent<Packet*, int>(lua_State* L, int eventName, Packet* const& arg1, const int& arg2);
template bool ScriptingEventManager::triggerEvent<>(lua_State* L, int eventName);



void ScriptingEventManager::registerHandler(lua_State* L, const int& eventName) {
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    eventHandlers[eventName].push_back({L, ref});
}



template<typename... Args>
bool ScriptingEventManager::triggerEvent(lua_State* L, int eventName, const Args&... args) {
    auto it = eventHandlers.find(eventName);
    if (it == eventHandlers.end()) return false;

    for (const auto& handler : it->second) {
        if (handler.luaState != L) {
            continue;
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, handler.ref);

        (LuaPusher::pushToLua(L, args), ...);

        if (lua_pcall(L, sizeof...(args), 1, 0) != LUA_OK) {
            std::string err = lua_tostring(L, -1);
            Logger::error("Error while executing event handler: {}", err);
            lua_pop(L, 1);
        }
    }
    if (lua_isboolean(L, -1)) {
        bool cancel = lua_toboolean(L, -1);
        lua_pop(L, 1);
        if (cancel) {
            return true;
        }
    } else {
        lua_pop(L, 1);
    }
    return false;
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