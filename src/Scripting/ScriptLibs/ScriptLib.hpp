#pragma once

#include <Scripting/ModuleScript.hpp>
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

    static D2D1_RECT_F toD2D1Rect(const luabridge::LuaRef& table) {
        if (!table.isTable() || table.length() < 4) {
            return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
        }
        if (!table[1].isNumber() || !table[2].isNumber() ||
            !table[3].isNumber() || !table[4].isNumber()) {
            return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
            }

        float left = table[1].cast<float>().value();
        float top = table[2].cast<float>().value();
        float right = table[3].cast<float>().value();
        float bottom = table[4].cast<float>().value();

        return D2D1::RectF(left, top, right, bottom);
    }

    static D2D1_COLOR_F toD2D1ColorF(const luabridge::LuaRef& table) {
        if (!table.isTable() || table.length() < 4) return D2D1::ColorF(D2D1::ColorF::White);

        if (!table[1].isNumber() || !table[2].isNumber() ||
            !table[3].isNumber() || !table[4].isNumber()) {
            return D2D1::ColorF(D2D1::ColorF::White);
            }

        float r = static_cast<float>(table[1].cast<int>().value()) / 255.0f;
        float g = static_cast<float>(table[2].cast<int>().value()) / 255.0f;
        float b = static_cast<float>(table[3].cast<int>().value()) / 255.0f;
        float a = static_cast<float>(table[4].cast<int>().value()) / 255.0f;

        // Return a Direct2D color
        return D2D1::ColorF(r, g, b, a);
    }
};