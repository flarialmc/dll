#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>


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
    const wchar_t* to_wchar_t(const std::string& str) {
        std::wstring wstr(str.begin(), str.end());
        return wstr.c_str();
    }



//    int lua_GUI_FlarialTextWithFont(lua_State *L) {
//        int x = luaL_checkinteger(L, 1);
//        int y = luaL_checkinteger(L, 2);
//
//        std::string text = luaL_checkstring(L, 3);
//
//        float width = luaL_checknumber(L, 4);
//        float height = luaL_checknumber(L, 5);
//        float fontSize = luaL_checknumber(L, 6);
//
//        FlarialGUI::FlarialTextWithFont(x, y, to_wchar_t(text), width,
//                                        height, DWRITE_TEXT_ALIGNMENT_LEADING,
//                                        fontSize,
//                                        DWRITE_FONT_WEIGHT_NORMAL);
//        return 0;
//    }

    int lua_GUI_RoundedHollowRect(lua_State *L) {
        float x = luaL_checkinteger(L, 1);
        float y = luaL_checkinteger(L, 2);
        auto color = static_cast<D2D1::ColorF *>(lua_touserdata(L, 3));

        _D3DCOLORVALUE d3dColorValue;
        d3dColorValue.r = color->r;
        d3dColorValue.g = color->g;
        d3dColorValue.b = color->b;
        d3dColorValue.a = color->a;

        float height = luaL_checknumber(L, 4);
        float width = luaL_checknumber(L, 5);
        float rounding = luaL_checknumber(L, 6);
        float shadowSize = luaL_checknumber(L, 7);
        FlarialGUI::ShadowRect(Vec2(x,y), Vec2(height, width), d3dColorValue, rounding, shadowSize);

        return 0;
    }

    void registerGUI(lua_State *L) {
        lua_newtable(L);
        lua_pushcfunction(L, lua_GUI_RoundedRect);
        lua_setfield(L, -2, "RoundedRect");

        lua_pushcfunction(L, lua_GUI_RoundedHollowRect);
        lua_setfield(L, -2, "RoundedHollowRect");

//        lua_pushcfunction(L, lua_GUI_FlarialTextWithFont);
//        lua_setfield(L, -2, "TextWithFont");

        lua_pushcfunction(L, lua_GUI_Color);
        lua_setfield(L, -2, "Color");        

        lua_setglobal(L, "GUI");
    }
}