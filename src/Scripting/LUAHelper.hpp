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

        LuaClass& registerLambdaFunction(const std::string& functionName, std::function<int(lua_State*)> handler) {
            auto wrapper = [](lua_State* L) -> int {
                auto* handlerPtr = static_cast<std::function<int(lua_State*)>*>(lua_touserdata(L, lua_upvalueindex(1)));
                if (handlerPtr) {
                    return (*handlerPtr)(L);
                } else {
                    //throw LuaException("push function type", "handler pointer is null did you return anything?");
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




    class LuaLib {
    private:
        lua_State* L;
        std::string libName;

    public:
        LuaLib(lua_State* state, const std::string& name)
                : L(state), libName(name) {
            lua_newtable(L);
        }

        inline LuaLib& registerFunction(const std::string& functionName, lua_CFunction func) {
            lua_pushcfunction(L, func);
            lua_setfield(L, -2, functionName.c_str());
            return *this;
        }

        inline LuaLib& registerLambdaFunction(const std::string& functionName, const std::function<int(lua_State*)>& handler) {
            auto wrapper = [](lua_State* L) -> int {
                auto* handlerPtr = static_cast<std::function<int(lua_State*)>*>(lua_touserdata(L, lua_upvalueindex(1)));
                if (handlerPtr) {
                    return (*handlerPtr)(L);
                } else {
                    //throw LuaException("registerLambdaFunction", "Handler pointer is null");
                }
            };

            auto* handlerPtr = new std::function<int(lua_State*)>(handler);
            lua_pushlightuserdata(L, handlerPtr);
            lua_pushcclosure(L, wrapper, 1);
            lua_setfield(L, -2, functionName.c_str());
            return *this;
        }

        ~LuaLib() {
            luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
            lua_pushvalue(L, -2);
            lua_setfield(L, -2, libName.c_str());
            lua_pop(L, 1);
        }
    };

    LuaLib getLib(const std::string& libName) {
        return LuaLib(L, libName);
    }


    class LuaDynamicClass {
    private:
        lua_State* L;
        std::string className;

        static int indexFunction(lua_State* L) {
            lua_getmetatable(L, 1);
            lua_pushvalue(L, 2);
            lua_rawget(L, -2);
            return 1;
        }

        static int newInstance(lua_State* L) {
            luaL_checktype(L, 1, LUA_TTABLE);
            lua_newtable(L);
            lua_pushvalue(L, 1);
            lua_setmetatable(L, -2);
            return 1;
        }

    public:
        LuaDynamicClass(lua_State* state, const std::string& name) : L(state), className(name) {
            luaL_newmetatable(L, className.c_str());

            lua_pushcfunction(L, indexFunction);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, newInstance);
            lua_setfield(L, -2, "new");
        }

        LuaDynamicClass& registerFunction(const std::string& functionName, lua_CFunction func) {
            lua_pushcfunction(L, func);
            lua_setfield(L, -2, functionName.c_str());
            return *this;
        }

        LuaDynamicClass& registerLambdaFunction(const std::string& functionName, std::function<int(lua_State*)> handler) {
            auto wrapper = [](lua_State* L) -> int {
                auto* handlerPtr = static_cast<std::function<int(lua_State*)>*>(lua_touserdata(L, lua_upvalueindex(1)));
                return handlerPtr ? (*handlerPtr)(L) : 0;
            };

            auto* handlerPtr = new std::function<int(lua_State*)>(handler);
            lua_pushlightuserdata(L, handlerPtr);
            lua_pushcclosure(L, wrapper, 1);
            lua_setfield(L, -2, functionName.c_str());
            return *this;
        }

        void pushInstanceToLua() {
            lua_newtable(L);
            luaL_getmetatable(L, className.c_str());
            lua_setmetatable(L, -2);
        }

        ~LuaDynamicClass() {
            lua_setglobal(L, className.c_str());
        }
    };

    std::shared_ptr<LuaDynamicClass> getDynamicClass() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return std::make_shared<LuaDynamicClass>(L, std::string("dc_" + std::to_string(timestamp)));
    }
};

namespace LuaPusher {
    inline void luaArray(lua_State* L, std::vector<float>& args) {
        lua_newtable(L);
        for (int i = 0; i < args.size(); i++) {
            //push item of array
            lua_pushnumber(L, args[i]);
            //set as item of lua array (+1 cuz lua arrays are stupid)
            lua_rawseti(L, -2, i+1);
        }
    }
    //overloads
    inline static void luaArray(lua_State* L, std::vector<std::string>& args) {
        lua_newtable(L);
        for (int i = 0; i < args.size(); i++) {
            lua_pushstring(L, args[i].c_str());
            lua_rawseti(L, -2, i+1);
        }
    }

    template<typename T>
    inline void pushToLua(lua_State* L, T arg) {
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
};