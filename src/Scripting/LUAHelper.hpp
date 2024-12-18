#pragma once

#include <lua.hpp>
#include <string>

class LuaException : public std::exception {
private:
    std::string whatwentwrong;
    std::string message;
public:
    explicit LuaException(const std::string wh, const std::string msg) : whatwentwrong(wh), message(msg) {
        Logger::error("Lua critical error while trying to " + wh + ", additional info: " + msg);
    }

    const char* what() const noexcept override {
        return std::string("Lua critical error while trying to " + whatwentwrong + ", additional info: " + message).c_str();
    }
};

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

        LuaClass& registerLambdaFunction(const std::string& functionName, std::function<int(lua_State*)> handler) {
            auto wrapper = [](lua_State* L) -> int {
                auto* handlerPtr = static_cast<std::function<int(lua_State*)>*>(lua_touserdata(L, lua_upvalueindex(1)));
                if (handlerPtr) {
                    return (*handlerPtr)(L);
                } else {
                    throw LuaException("push function type", "handler pointer is null did you return anything?");
                }
            };

            auto* handlerPtr = new std::function<int(lua_State*)>(handler);
            lua_pushlightuserdata(L, handlerPtr);
            lua_pushcclosure(L, wrapper, 1);
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
};