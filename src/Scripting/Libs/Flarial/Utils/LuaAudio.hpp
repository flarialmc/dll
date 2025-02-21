#pragma once

#include "../lib/miniaudio.h"
#include "lib/lua/lua-5.4.7/include/lua.hpp"
#include <d2d1helper.h>
#include "src/Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>
#include "src/SDK/Client/Actor/Player.hpp"
#include "src/Scripting/LUAHelper.hpp"
#include "SDK/SDK.hpp"
#include "Utils/Audio.hpp"
#include <algorithm>

namespace LuaAudio {
    int lua_playSound(lua_State *L){
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        std::string sound = luaL_checkstring(L, 1);

        Audio::play(const_cast<char*>(sound.c_str()));

        return 0;
    }

    void registerLib(lua_State *L) {

        LUAHelper(L)
                .getLib("Audio")
                .registerFunction("playSound", lua_playSound);
    }
}