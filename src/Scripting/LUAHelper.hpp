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

        void luaArray(std::vector<int>& args) {
            lua_newtable(L);
            for (int i = 0; i < args.size(); i++) {
                //push item of array
                lua_pushinteger(L, args[i]);
                //set as item of lua array (+1 cuz lua arrays are stupid)
                lua_rawseti(L, -2, i+1);
            }
        }
        //overloads
        void luaArray(std::vector<std::string>& args) {
            lua_newtable(L);
            for (int i = 0; i < args.size(); i++) {
                lua_pushstring(L, args[i].c_str());
                lua_rawseti(L, -2, i+1);
            }
        }
        ~LuaClass() {
            lua_setglobal(L, className.c_str());
        }
    };

    LuaClass getClass(const std::string& className) {
        return LuaClass(L, className);
    }
};