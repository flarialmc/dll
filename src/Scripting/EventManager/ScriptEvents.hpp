#pragma once

#include "lua.h"

namespace ScriptEvents {

    enum EventType {
        onRenderEvent = 1,
        onTickEvent = 2,
        onPacketReceiveEvent = 3,
        onMouseEvent = 4,
        onKeyEvent = 5
    };

    void pushEventTypesToLua(lua_State *L) {
        lua_newtable(L);

        lua_pushinteger(L, onRenderEvent);
        lua_setfield(L, -2, "onRenderEvent");

        lua_pushinteger(L, onTickEvent);
        lua_setfield(L, -2, "onTickEvent");

        lua_pushinteger(L, onPacketReceiveEvent);
        lua_setfield(L, -2, "onPacketReceiveEvent");

        lua_pushinteger(L, onMouseEvent);
        lua_setfield(L, -2, "onMouseEvent");

        lua_pushinteger(L, onKeyEvent);
        lua_setfield(L, -2, "onKeyEvent");



        lua_setglobal(L, "EventType");
    }
}
