#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>
#include "../../SDK/Client/Actor/Player.hpp"
#include "../LUAHelper.hpp"
#include <algorithm>

#include "Player.hpp"

namespace player {
    int effectToPath(lua_State* L) {
        if (UnifiedMobEffectData* data=static_cast<UnifiedMobEffectData *>(lua_touserdata(L, -1)); data->isValid()) {
            lua_pushstring(L,data->getTexturePath().c_str());
        }
        return 1;
    }
    int effectTime(lua_State* L) {
        if (UnifiedMobEffectData* data=static_cast<UnifiedMobEffectData *>(lua_touserdata(L, -1)); data->isValid()) {
            lua_pushstring(L,data->getTime().c_str());
        }
        return 1;
    }


    void registerEffects(lua_State *L) {

        LUAHelper(L)
                .getClass("MobEffects")
                .registerFunction("getPlayerName", lua_getPlayerName)
                .registerFunction("getX", lua_getPlayerX)
                .registerFunction("getY", lua_getPlayerY)
                .registerFunction("getZ", lua_getPlayerZ)
                .registerFunction("getSlot", lua_getPlayerSlot)
                .registerFunction("returnPosArray", lua_returnPosArray)
                .registerFunction("returnPlayerList", lua_returnPlayerList)
                .registerFunction("isOnGround", lua_isOnGround);
    }
}
