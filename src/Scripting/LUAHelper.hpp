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