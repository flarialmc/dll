#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../LUAHelper.hpp"
#include <string>
#include <codecvt>


namespace GUI {

    int lua_Color(lua_State *L) {
        float r = luaL_checknumber(L, 1);
        float g = luaL_checknumber(L, 2);
        float b = luaL_checknumber(L, 3);
        float a = luaL_checknumber(L, 4);
        auto color = new D2D1::ColorF(r, g, b, a);

        lua_pushlightuserdata(L, color);
        return 1;
    }

    int lua_RoundedRect(lua_State *L) {
        int x = luaL_checkinteger(L, 1);
        int y = luaL_checkinteger(L, 2);
        auto color = static_cast<D2D1::ColorF *>(lua_touserdata(L, 3));

        float width = luaL_checknumber(L, 4);
        float height = luaL_checknumber(L, 5);
        float radiusX = luaL_checknumber(L, 6);
        float radiusY = luaL_checknumber(L, 7);

        FlarialGUI::RoundedRect(x, y, *color, radiusX, radiusY, width, height);

        return 0;
    }
    int lua_drawImage(lua_State *L) {
        std::string path = luaL_checkstring(L, 1);
        //x, y, z
        FlarialGUI::image(path, D2D1::RectF(luaL_checknumber(L,3),luaL_checknumber(L,4),luaL_checknumber(L,5)), luaL_checkstring(L,2));
        return 1;
    }


    int lua_FlarialTextWithFont(lua_State *L) {
        int x = luaL_checkinteger(L, 1);
        int y = luaL_checkinteger(L, 2);

        std::string text = luaL_checkstring(L, 3);

        float width = luaL_checknumber(L, 4);
        float height = luaL_checknumber(L, 5);
        float fontSize = luaL_checknumber(L, 6);


        FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), width * 3.0f, height,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        fontSize,
                                        DWRITE_FONT_WEIGHT_NORMAL);
        return 0;
    }

    int lua_RoundedHollowRect(lua_State *L) {
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

    int lua_RoundedButton(lua_State *L) {
        int index = luaL_checkinteger(L, 1);
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        auto color = static_cast<D2D1::ColorF *>(lua_touserdata(L, 4));
        auto textColor = static_cast<D2D1::ColorF *>(lua_touserdata(L, 5));
        std::wstring text = FlarialGUI::to_wide(luaL_checkstring(L, 6));
        float width = luaL_checknumber(L, 7);
        float height = luaL_checknumber(L, 8);
        float radiusX = luaL_checknumber(L, 9);
        float radiusY = luaL_checknumber(L, 10);
        bool glow = lua_toboolean(L, 11);

        bool result = FlarialGUI::RoundedButton(index, x, y, *color, *textColor, text.c_str(), width, height, radiusX, radiusY, glow);

        lua_pushboolean(L, result);
        return 1;
    }

    int lua_NormalRender(lua_State *L) {
        Module* script = Scripting::getModuleByState(L);

        int index = luaL_checkinteger(L, 1);
        std::string text = luaL_checkstring(L, 2);

        script->normalRender(index, text);

        return 0;
    }

    void registerGUI(lua_State *L) {
        LUAHelper(L)
                .getClass("GUI")
                .registerFunction("RoundedRect", lua_RoundedRect)
                .registerFunction("RoundedHollowRect", lua_RoundedHollowRect)
                .registerFunction("TextWithFont", lua_FlarialTextWithFont)
                .registerFunction("Color", lua_Color)
                .registerFunction("RoundedButton", lua_RoundedButton)
                .registerFunction("NormalRender", lua_NormalRender)
                .registerFunction("drawImage", lua_drawImage)
                .registerLambdaFunction("getCurrentScreen",[](lua_State* L) -> int {
                    lua_pushstring(L, SDK::getCurrentScreen().c_str());
                    return 1;
                });
    }
}