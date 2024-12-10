#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/GUI/Engine/Constraints.hpp"
#include <string>
#include <codecvt>
#include <cmath>

namespace LuaConstraints {
    int lua_PercentageConstraint(lua_State *L) {
        float percentage = luaL_checknumber(L, 1);
        std::string edge = luaL_checkstring(L, 2);
        bool ignore_stack = lua_toboolean(L, 3);

        float value = Constraints::PercentageConstraint(percentage, edge, ignore_stack);

        lua_pushnumber(L, value);
        return 1;
    }

    int lua_RelativeConstraint(lua_State *L) {
        float percent = luaL_checknumber(L, 1);
        std::string dimension = luaL_checkstring(L, 2);
        bool ignore_stack = lua_toboolean(L, 3);

        float value = Constraints::RelativeConstraint(percent, dimension, ignore_stack);

        lua_pushnumber(L, value);
        return 1;
    }

    int lua_CenterConstraint(lua_State *L) {
        float width = luaL_checknumber(L, 1);
        float height = luaL_checknumber(L, 2);
        std::string axis = luaL_checkstring(L, 3);
        float xModifier = luaL_optnumber(L, 4, 0.0f);
        float yModifier = luaL_optnumber(L, 5, 0.0f);
        bool ignore_stack = lua_toboolean(L, 6);

        Vec2<float> position = Constraints::CenterConstraint(width, height, axis, xModifier, yModifier, ignore_stack);

        lua_newtable(L);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");

        return 1;
    }

    int lua_RoundingConstraint(lua_State *L) {
        float radiusX = luaL_checknumber(L, 1);
        float radiusY = luaL_checknumber(L, 2);

        Vec2<float> radius = Constraints::RoundingConstraint(radiusX, radiusY);

        lua_newtable(L);
        lua_pushnumber(L, radius.x);
        lua_setfield(L, -2, "radiusX");
        lua_pushnumber(L, radius.y);
        lua_setfield(L, -2, "radiusY");

        return 1;
    }

    int lua_FontScaler(lua_State *L) {
        float dimension = luaL_checknumber(L, 1);

        float scaledFontSize = Constraints::FontScaler(dimension);

        lua_pushnumber(L, scaledFontSize);
        return 1;
    }

    int lua_CalculatePercentage(lua_State *L) {
        float x = luaL_checknumber(L, 1);
        float y = luaL_checknumber(L, 2);
        float adjustWidth = luaL_optnumber(L, 3, 0.0f);
        float adjustHeight = luaL_optnumber(L, 4, 0.0f);

        Vec2<float> percentage = Constraints::CalculatePercentage(x, y, adjustWidth, adjustHeight);

        lua_newtable(L);
        lua_pushnumber(L, percentage.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, percentage.y);
        lua_setfield(L, -2, "y");

        return 1;
    }

    void registerConstraints(lua_State *L) {
        LUAHelper(L)
                .getClass("Constraints")
                .registerFunction("PercentageConstraint", lua_PercentageConstraint)
                .registerFunction("RelativeConstraint", lua_RelativeConstraint)
                .registerFunction("CenterConstraint", lua_CenterConstraint)
                .registerFunction("RoundingConstraint", lua_RoundingConstraint)
                .registerFunction("FontScaler", lua_FontScaler)
                .registerFunction("CalculatePercentage", lua_CalculatePercentage);
    }
}
