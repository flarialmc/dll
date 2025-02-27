#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <SDK/SDK.hpp>
#include <SDK/Client/Actor/Actor.hpp>

class PlayerLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            auto lp = SDK::clientInstance->getLocalPlayer();
            if (!lp) return 0;

            auto position = lp->getPosition();
            if (!position) return 0;

            lua_newtable(L);
            lua_pushnumber(L, position->x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, position->y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, position->z);
            lua_setfield(L, -2, "z");

            return 1;
        }, "getPosition", "player");
    }
};