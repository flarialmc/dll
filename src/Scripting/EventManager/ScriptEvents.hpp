#pragma once

#include "lua.h"
#include <magic_enum/magic_enum_all.hpp>
#include <string>

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

        for (auto [event, name] : magic_enum::enum_entries<EventType>()) {
            lua_pushinteger(L, static_cast<lua_Integer>(event));
            lua_setfield(L, -2, name.data());
        }

        lua_setglobal(L, "EventType");
    }
}
