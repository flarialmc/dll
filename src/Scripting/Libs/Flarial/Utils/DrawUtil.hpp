#pragma once

#include "lua.hpp"
#include "Scripting/LUAHelper.hpp"
#include "Utils/Render/DrawUtils.hpp"

namespace LuaDrawUtil {

    inline int lua_DrawBox(lua_State* L) {
        float lowerX = static_cast<float>(lua_tonumber(L, 1));
        float lowerY = static_cast<float>(lua_tonumber(L, 2));
        float lowerZ = static_cast<float>(lua_tonumber(L, 3));
        float upperX = static_cast<float>(lua_tonumber(L, 4));
        float upperY = static_cast<float>(lua_tonumber(L, 5));
        float upperZ = static_cast<float>(lua_tonumber(L, 6));
        float lineWidth = static_cast<float>(lua_tonumber(L, 7));
        int mode = static_cast<int>(lua_tointeger(L, 8));

        auto color = static_cast<D2D1::ColorF *>(lua_touserdata(L, 9));

        _D3DCOLORVALUE d3dColorValue;
        d3dColorValue.r = color->r;
        d3dColorValue.g = color->g;
        d3dColorValue.b = color->b;
        d3dColorValue.a = color->a;

        DrawUtils::addBox(Vec3<float>(lowerX, lowerY, lowerZ), Vec3<float>(upperX, upperY, upperZ), lineWidth, mode, d3dColorValue);
        return 0;
    }

    void registerLib(lua_State* L) {
        LUAHelper(L)
                .getLib("Flarial/Utils/DrawUtil")
                .registerFunction("drawBox", lua_DrawBox);
    }
}