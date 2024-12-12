#pragma once

#include <lua.hpp>
#include <string>

class LUAHelper {
private:
    lua_State* L;

public:
    explicit LUAHelper(lua_State* state) : L(state) {}



    class LuaClass {
    private:
        lua_State* L;
        std::string className;

    public:
        LuaClass(lua_State* state, const std::string& name) : L(state), className(name) {
            lua_newtable(L);
        }

        LuaClass& registerFunction(const std::string& functionName, lua_CFunction func) {
            lua_pushcfunction(L, func);
            lua_setfield(L, -2, functionName.c_str());
            return *this;
        }

        ~LuaClass() {
            lua_setglobal(L, className.c_str());
        }
    };

    LuaClass getClass(const std::string& className) {
        return LuaClass(L, className);
    }
};

namespace LuaPusher {
    void luaArray(lua_State* L, std::vector<float>& args) {
        lua_newtable(L);
        for (int i = 0; i < args.size(); i++) {
            //push item of array
            lua_pushnumber(L, args[i]);
            //set as item of lua array (+1 cuz lua arrays are stupid)
            lua_rawseti(L, -2, i+1);
        }
    }
    //overloads
    static void luaArray(lua_State* L, std::vector<std::string>& args) {
        lua_newtable(L);
        for (int i = 0; i < args.size(); i++) {
            lua_pushstring(L, args[i].c_str());
            lua_rawseti(L, -2, i+1);
        }

    }
    //using auto for light user data etc..
    static void luaArray(lua_State* L, std::vector<auto>& args) {
        lua_newtable(L);
        for (int i = 0; i < args.size(); i++) {
            lua_pushlightuserdata(L, args[i]);
            lua_rawseti(L, -2, i+1);
        }

    }
    static void luaTable(lua_State* L, std::unordered_map<std::string, std::string>& args) {
        lua_newtable(L);
        for (const auto& e: args) {
            lua_pushstring(L, e.first.c_str());
            lua_pushstring(L, e.second.c_str());
            lua_settable(L, -3);
        }
    }
    static void luaTable(lua_State* L, std::unordered_map<std::string, float>& args) {
        lua_newtable(L);
        for (const auto& e: args) {
            lua_pushstring(L, e.first.c_str());
            lua_pushnumber(L, e.second);
            lua_settable(L, -3);
        }
    }
};