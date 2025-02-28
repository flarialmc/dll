#pragma once

#include <Scripting/ScriptModuleBase.hpp>
#include <LuaBridge/LuaBridge.h>

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

    static ImVec2 toImVec2(const luabridge::LuaRef& table) {
        if (!table.isTable() || table.length() < 2) return {0, 0};
        if (!table[1].isNumber() || !table[2].isNumber()) return {0, 0};

        float x = table[1].cast<float>().value();
        float y = table[2].cast<float>().value();

        return {x, y};
    }

    static ImVec4 toImVec4(const luabridge::LuaRef& table) {
        if (!table.isTable() || table.length() < 4) return {0, 0, 0, 0};
        if (!table[1].isNumber() || !table[2].isNumber() || !table[3].isNumber() || !table[4].isNumber()) return {0, 0, 0, 0};

        float x = table[1].cast<float>().value();
        float y = table[2].cast<float>().value();
        float z = table[3].cast<float>().value();
        float w = table[4].cast<float>().value();

        return {x, y, z, w};
    }

    static ImU32 toColor(const luabridge::LuaRef& table) {
        if (!table.isTable() || table.length() < 4) return IM_COL32(255, 255, 255, 255);
        if (!table[1].isNumber() || !table[2].isNumber() || !table[3].isNumber() || !table[4].isNumber()) return IM_COL32(255, 255, 255, 255);

        int r = table[1].cast<int>().value();
        int g = table[2].cast<int>().value();
        int b = table[3].cast<int>().value();
        int a = table[4].cast<int>().value();

        return IM_COL32(r, g, b, a);
    }
};