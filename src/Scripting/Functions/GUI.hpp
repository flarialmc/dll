#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"


namespace GUI {

    int lua_GUI_Color(lua_State *L) {
        float r = luaL_checknumber(L, 1);
        float g = luaL_checknumber(L, 2);
        float b = luaL_checknumber(L, 3);
        float a = luaL_checknumber(L, 4);
        auto color = new D2D1::ColorF(r, g, b, a);

        lua_pushlightuserdata(L, color);
        return 1;
    }

    int lua_GUI_RoundedRect(lua_State *L) {
        int x = luaL_checkinteger(L, 1);
        int y = luaL_checkinteger(L, 2);
        auto color = static_cast<D2D1::ColorF *>(lua_touserdata(L, 3));

        float radiusX = luaL_checknumber(L, 4);
        float radiusY = luaL_checknumber(L, 5);
        float width = luaL_checknumber(L, 6);
        float height = luaL_checknumber(L, 7);

        FlarialGUI::RoundedRect(x, y, *color, width, height, radiusX, radiusY);

        return 0;
    }

    void registerGUI(lua_State *L) {
        lua_newtable(L);
        lua_pushcfunction(L, lua_GUI_RoundedRect);
        lua_setfield(L, -2, "RoundedRect");

        lua_pushcfunction(L, lua_GUI_Color);
        lua_setfield(L, -2, "Color");

        lua_setglobal(L, "GUI");
    }
}