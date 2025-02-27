#pragma once

#include <Scripting/ScriptModuleBase.hpp>

class ScriptLib {
public:
    ScriptLib() = default;
    virtual ~ScriptLib() = default;

    virtual void initialize(lua_State* L) {}

    template <typename T>
    static void registerLib(lua_State* L) {
        T lib;
        lib.initialize(L);
    }
protected:
    struct LambdaWrapper {
        std::function<int(lua_State*)> fn;
    };

    static int lambdaTrampoline(lua_State* L);
    static void registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName, const char* tablePath);
    static void registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName);
    static void createNestedTable(lua_State* L, const std::string& path);
};