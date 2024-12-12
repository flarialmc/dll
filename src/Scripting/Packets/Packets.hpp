#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/Packet.hpp"
#include <magic_enum/magic_enum_all.hpp>
#include <string>

namespace LuaPackets {
    void pushPacketTypesToLua(lua_State *L) {
        lua_newtable(L);

        for (auto [packet, name] : magic_enum::enum_entries<MinecraftPacketIds>()) {
            lua_pushinteger(L, static_cast<lua_Integer>(packet));
            lua_setfield(L, -2, name.data());
        }

        lua_setglobal(L, "MinecraftPacketIds");
    }
}
