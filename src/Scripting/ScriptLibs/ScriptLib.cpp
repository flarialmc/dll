#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>

int ScriptLib::lambdaTrampoline(lua_State* L) {
    auto* wrapper = static_cast<LambdaWrapper*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!wrapper) {
        luaL_error(L, "[ScriptLib] Missing LambdaWrapper upvalue");
        return 0;
    }

    return wrapper->fn(L);
}

void ScriptLib::registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName,const char* tablePath) {
    createNestedTable(L, tablePath);

    auto* wrapper = new LambdaWrapper{ callback };

    lua_pushlightuserdata(L, wrapper);
    lua_pushcclosure(L, &lambdaTrampoline, 1);

    lua_setfield(L, -2, functionName);

    lua_pop(L, 1);
}

void ScriptLib::registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName) {
    auto* wrapper = new LambdaWrapper{ callback };
    lua_pushlightuserdata(L, wrapper);
    lua_pushcclosure(L, &lambdaTrampoline, 1);
    lua_setglobal(L, functionName);
}

void ScriptLib::createNestedTable(lua_State* L, const std::string& path) {
    if (path.empty()) {
        lua_pushglobaltable(L);
        return;
    }

    const auto tokens = String::split(path, '.');

    lua_pushglobaltable(L);

    for (auto& segment : tokens) {
        lua_getfield(L, -1, segment.c_str());
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setfield(L, -2, segment.c_str());
            lua_getfield(L, -1, segment.c_str());
        }
        lua_remove(L, -2);
    }
}